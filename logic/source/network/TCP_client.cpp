#include "Network.hpp"
#include "TCP_client.hpp"

TCP_client::TCP_client() : is_polling(false)
{
    // Start the receive thread
    socket_fdesc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fdesc < 0) {
        throw std::runtime_error("Socket_wrapper_TCP: Failed to create socket");
    }

    int flags = fcntl(socket_fdesc, F_GETFL, 0); 
    if (fcntl(socket_fdesc, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        close(socket_fdesc);
        throw std::runtime_error("Socket_wrapper_TCP: Failed to enable non-blocking mode");
    }
}

TCP_client::~TCP_client() {
    disconnect();
    close(socket_fdesc);
}

bool TCP_client::connect_to(const Socket_info& destination, int n_attempts, int ms_between) {
    if(is_polling) {
        disconnect();
    }
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(destination.port);
    server_addr.sin_addr.s_addr = destination.address;

    bool success = false;
    // Connect to server
    for (int i = 0; i < n_attempts; i++) {
        if (connect(socket_fdesc, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
            success = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ms_between));
    }
    if (!success) {
        return false;
    }
    // Await confirmation
    if (handshake_timeout_ms > 0) {
        sockaddr_in inbound_addr;
        socklen_t inbound_addr_len = sizeof(inbound_addr);
    
        pollfd poll_req = {socket_fdesc, POLLIN, 0};
        int poll_res = poll(&poll_req, 1, handshake_timeout_ms);
        if (poll_res > 0) {
            success = true;
        }   
    }  
    // Launch receiver thread
    server = destination;
    is_polling = true;

    receiver_thread = std::thread(&TCP_client::receive_messages, this);
    return true;
}

bool TCP_client::disconnect()
{
    std::lock_guard<std::mutex> lock_conf(confirmation_mutex);
    if (!is_polling) {
        return false;
    }

    is_polling = false;
    bool was_connected = server.has_value();
    server = {};

    if (receiver_thread.joinable()) {
        receiver_thread.join();
    }
    return was_connected;
}

const std::optional<Socket_info> TCP_client::get_connection()
{
    std::lock_guard<std::mutex> conf_lock(confirmation_mutex);
    return server;
}

bool TCP_client::send_msg(const std::vector<char>& msg) {
    std::lock_guard<std::mutex> lock(receiver_mutex);
    if (server.has_value()) {
        return send(socket_fdesc, msg.data(), msg.size(), 0) == msg.size();
    }
    else {
        return false;
    }
}

Socket_inbound_message TCP_client::get_message() {
    std::lock_guard<std::mutex> lock(receiver_mutex);
    if (message_queue.empty()) {
        return {};
    }
    Socket_inbound_message message = message_queue.front();
    message_queue.pop();
    return message;
}


void TCP_client::receive_messages() {
    sockaddr_in inbound_addr;
    socklen_t inbound_addr_len = sizeof(inbound_addr);
    
    pollfd poll_req = {socket_fdesc, POLLIN, 0};

    char buffer[SOCKET_BUFFER_SIZE];
    while (is_polling) {
        poll(&poll_req, 1, -1);
        {
        std::lock_guard<std::mutex> lock(receiver_mutex);
        if (message_queue.size() < TCP_CLIENT_MAX_INBOX) {
            int bytes_received = recvfrom(socket_fdesc, buffer, sizeof(buffer), \
                                0, (sockaddr*)&inbound_addr, &inbound_addr_len);
            if (bytes_received > 0) {
                std::vector<char> msg = std::vector<char>(buffer, buffer + bytes_received);
                message_queue.push({{ntohs(inbound_addr.sin_port), inbound_addr.sin_addr.s_addr}, msg});
            }
        }
        }  
    }
}
