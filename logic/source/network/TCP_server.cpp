#include "Socket_wrappers.hpp"

TCP_server::TCP_server(u_short port) : own_port(port), polled(0) 
{
    int socket_fdesc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fdesc == 0) {
        std::cerr << "Socket failed\n";
        exit(EXIT_FAILURE);
    }
    polled.push_back({socket_fdesc, POLLIN, 0});

    int opt = 1;
    if (setsockopt(socket_fdesc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        close(socket_fdesc);
        throw std::runtime_error("TCP_server: failed to set socket option");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(own_port);

    if (bind(socket_fdesc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(socket_fdesc);
        throw std::runtime_error("TCP_server: failed to bind socket");
    }

    if (listen(socket_fdesc, TCP_SERVER_MAX_REQ) < 0) {
        close(socket_fdesc);
        throw std::runtime_error("TCP_server: failed to bind socket");
    }

    // Set server socket to non-blocking mode
    fcntl(socket_fdesc, F_SETFL, O_NONBLOCK);
    is_running = true;
    receiver_thread = std::thread(&TCP_server::poll_events, this);
}

TCP_server::~TCP_server() 
{
        {
    std::lock_guard<std::mutex> lock_socket(socket_mutex);
    std::lock_guard<std::mutex> lock_plist(peer_list_mutex);

    for (const pollfd& pd : polled) {
        close(pd.fd);
    }
        }
    is_running = false;
    if (receiver_thread.joinable()) {
        receiver_thread.join();
    }
}

void TCP_server::poll_events() {
    while (is_running) {
        int activity = poll(polled.data(), polled.size(), -1);
        if (activity < 0 && errno != EINTR) {
            throw std::runtime_error("Polling error");
        }

        if (polled[0].revents & POLLIN) { // New connection request
            handle_request();
        }

        for (int id=1; id < polled.size(); id++) { // Handle client message
            if (polled[id].revents & POLLIN) {
                handle_client(id-1);
            }
        }
    }
}

void TCP_server::handle_request() 
{
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    int new_socket = accept(polled[0].fd, (struct sockaddr*)&client_addr, &addrlen);
    if (new_socket < 0) {
        throw std::runtime_error("TCP_server: Failed to accept connection");
    }

    std::lock_guard lock_allowed_list(allowed_list_mutex);
    if ((accept_all || allowed_ips.find(client_addr.sin_addr.s_addr) != allowed_ips.end()) && \
        polled.size() < TCP_SERVER_MAX_CLIENTS) 
    {
        std::lock_guard lock(peer_list_mutex);
        // send confirmation message
        std::vector<char> confirmation_message = {0};
        if (send(new_socket, confirmation_message.data(), confirmation_message.size(), 0) != confirmation_message.size()) {
            close(new_socket);
            return;
        }

        polled.push_back({new_socket, POLLIN, 0});

        Socket_info new_info = {ntohs(client_addr.sin_port), client_addr.sin_addr.s_addr};
        peer_info.push_back(new_info);

        if (connection_log.size() < TCP_SERVER_MAX_LOG_QUEUE) {
            connection_log.push(new_info);
        }  
    } 
    else {
        close(new_socket); // Reject connection
    }
}

void TCP_server::handle_client(int id) 
{ 
    std::lock_guard<std::mutex> lock_socket(socket_mutex);

    int client_socket = polled[id+1].fd;
    char buffer[SOCKET_BUFFER_SIZE] = {};
    int bytes_read = read(client_socket, buffer, sizeof(buffer));

    std::lock_guard lock_inbox(inbox_mutex);
    if (bytes_read > 0 && inbox.size() < TCP_SERVER_MAX_INBOX) {
        inbox.push({peer_info[id], {buffer, buffer+bytes_read}});
    }
}

void TCP_server::allow_ip(in_addr_t addr)
{
    std::lock_guard lock_allowed_list(allowed_list_mutex);
    allowed_ips.insert(addr);
}

void TCP_server::disallow_ip(in_addr_t addr)
{
    std::lock_guard lock_allowed_list(allowed_list_mutex);
    allowed_ips.erase(addr);
}

Socket_inbound_message TCP_server::get_message()
{
    std::lock_guard<std::mutex> lock(inbox_mutex);
    if(inbox.empty()) {
        return {};
    }
    else {
        Socket_inbound_message msg = inbox.front();
        inbox.pop();
        return msg;
    }
}

const Socket_info TCP_server::get_connection_event()
{
    std::lock_guard<std::mutex> lock(connection_log_mutex);
    if (connection_log.empty()) {
        return {};
    }
    else {
        Socket_info info = connection_log.front();
        connection_log.pop();
        return info;
    }
}

bool TCP_server::disconnect_peer(int id) 
{ 
    std::lock_guard<std::mutex> lock_socket(socket_mutex);
    std::lock_guard<std::mutex> lock_plist(peer_list_mutex);

    if (id >= polled.size()-1 || id < 0) {
        throw std::out_of_range("TCP_server::disconnect(): id out of range");
    }
    int fd = polled[id+1].fd;

    if (close(fd) < 0) {
        return false;
    }
    else { // Socket closed successfully
        polled.erase(polled.begin() + 1 + id);
        peer_info.erase(peer_info.begin() + 1 + id);
        return true;
    }
}

bool TCP_server::disconnect_peer_addr(Socket_info addr) 
{ 
    std::lock_guard<std::mutex> lock_socket(socket_mutex);
    std::lock_guard<std::mutex> lock_plist(peer_list_mutex);

    int index = find_index(peer_info, addr);

    if (index == -1) {
        return false;
    }

    if (close(polled[index+1].fd) < 0) {
        return false;
    }

    polled.erase(polled.begin()+index+1);
    peer_info.erase(peer_info.begin()+index);
    return true;
}

bool TCP_server::send_to(int id, std::vector<char> msg)
{
    std::lock_guard<std::mutex> lock(socket_mutex);
    if (id < 0 || id >= polled.size()-1) {
        throw std::out_of_range("TCP_server::send_to(): id out of range");
    }
    return send(polled[id+1].fd, msg.data(), msg.size(), 0) == msg.size();
}

bool TCP_server::send_all(std::vector<char> message)
{
    bool success = true;
    std::lock_guard<std::mutex> socket_lock(socket_mutex), polled_lock(peer_list_mutex);
    for (int i = 1; i < polled.size(); i++) {
        success &= send(polled[i].fd, message.data(), message.size(), 0) == message.size();
    }
    return success;
}