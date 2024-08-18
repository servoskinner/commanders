#pragma once

#include "Misc_functions.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstdlib>
#include <cstring>

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <optional>
#include <set>
#include <unordered_map>

#define SOCKET_BUFFER_SIZE           1024
#define BROADCAST_IP                 "255.255.255.255"
#define DISCOVER_REPLY_SIZE          sizeof(Server_info)+1

#define TCP_CLIENT_MAX_INBOX         1024
#define TCP_SERVER_MAX_LOG_QUEUE     256
#define TCP_SERVER_MAX_INBOX         1024
#define TCP_SERVER_MAX_REQ           64
#define TCP_SERVER_MAX_CLIENTS       1023

enum inet_message_tags
{
    MSG_CONTROL,
    MSG_GAMEPLAY,
    MSG_CHAT
}; 
enum inet_control_tags
{
    ICTRL_ACK,
    ICTRL_NACK,
    ICTRL_UPKEEP,
    ICTRL_DISCOVER,
    ICTRL_DISCOVER_REPLY,
    ICTRL_CONNECTION_REQUEST,
    ICTRL_SERVER_FORCE_DISCONNECT
};

/**
 * @struct Information offered by server when discovered.
 */
struct Server_info
{
    char description[128];
    char connected_players;
    char flags[8];
};

/**
 * @struct Combination of a socket's port and IP address.
 */
struct Socket_info
{
    u_short port = 0;
    in_addr_t address = 0;

    Socket_info(u_short n_port, const char *addrstr = "127.0.0.1") : port(n_port), address(inet_addr(addrstr)) {}
    Socket_info(u_short n_port, std::string addrstr = "127.0.0.1") : port(n_port), address(inet_addr(addrstr.c_str())) {}
    Socket_info(u_short n_port, in_addr_t addr) : port(n_port), address(addr) {}
    Socket_info() : port(0), address(0) {}

    const bool operator==(const Socket_info& other) {return port == other.port && address == other.address; }
    const std::string addrstr();
};

/**
 * @struct Message in form of byte vector and its sender.
 */
struct Socket_inbound_message
{
    Socket_info sender = {};
    std::vector<char> msg = {};
};

/**
 * @class Entity used to send and receive byte arrays over network using UDP.
 */
class UDP_wrapper
{
    public:
    UDP_wrapper(u_short port);
    ~UDP_wrapper() { close(socket_fdesc);}

    const Socket_inbound_message get_message();
    const bool send_to(const Socket_info& dest, const std::vector<char>& msg);

    const inline u_short get_port() { return own_port;};

    protected:
    u_short own_port = -1;
    int socket_fdesc = -1;
};

class TCP_client
{
    public:
    TCP_client();
    ~TCP_client();

    Socket_inbound_message get_message();
    bool send_msg(const std::vector<char>& msg);
    bool connect_to(const Socket_info& destination);
    bool disconnect();
    const std::optional<Socket_info> get_connection();

    private:
    void receive_messages();

    u_short own_port = -1;
    int socket_fdesc = -1;

    std::thread receiver_thread;
    std::atomic<bool> is_polling;
    std::atomic<bool> connected;
    std::queue<Socket_inbound_message> message_queue;
    std::optional<Socket_info> server = {};
    std::mutex receiver_mutex, confirmation_mutex;
};

class TCP_server
{
    public:
    std::set<in_addr_t> allowed_ips; ///< IP addresses that can connect to the server.

    /** If set to true, accepts all connection requests
     *  without checking sender IP against allowed_ips.
     */
    bool accept_all = false;

    /**
     *  @brief Constructor of TCP_server.
     *  @param port port to host the service socket used to request new connections.
     */
    TCP_server(u_short port);
    ~TCP_server();

    /**
     *  @brief Run by a separate thread: checks for incoming messages on open sockets and processes them.
     */
    void poll_events();

    /**
     *  @brief Allows connection requests from clients with given IP address.
     */
    void allow_ip(in_addr_t addr);

    /**
     *  @brief Forbids connection requests from clients with given IP address.
     */
    void disallow_ip(in_addr_t addr);
    
    /**
     *  @brief Returns a set of IPs that are allowed to connect.
     */
    const std::set<in_addr_t> get_allowed_ips() { return allowed_ips;}
    
    /**
     *  @brief Returns port that handles connection requests.
     */
    const u_short get_port() { return own_port;}

    /**
     *  @brief Pops first value from queue that holds Socket_infos of recently connected peers.
     */
    const Socket_info get_connection_event();
    
    inline const std::vector<Socket_info> get_peers() { return peer_info;}

    /**
     *  @brief Sends message to peer with given id.
     *
     *  @param id id of peer to send the message to.
     *  @param message byte vector to send.
     *  @return Whether all bytes were delivered successfully.
    */
    bool send_to(int id, std::vector<char> message);

    /**
     *  @brief Sends message to all connected peers.
     *
     *  @param message byte vector to send.
     *  @return Whether all send operations were successful.
    */
    bool send_all(std::vector<char> message);
    
     /**
     *  @brief Retrieves a received message from the queue.
     * 
     *  @return The message itself as byte vector
     *  combined with sender's port and IP address. 
    */
    Socket_inbound_message get_message();

    /**
     *  @brief Terminates connection with a peer.
     * 
     *  @param id id of peer. Addresses can be accessed using get_peers().
     *  @return Whether terminating connection was successful.
    */
    bool disconnect_peer(int id);
    /**
     *  @brief Terminates connection with a peer with matching IP address and port.
     * 
     *  @param addr Address and port of peer. Addresses can be accessed using get_peers().
     *  @return Whether terminating connection was successful.
    */
    bool disconnect_peer_addr(Socket_info peer_info);

    protected:
    u_short own_port; ///< Port to receive connection requests.

    std::vector<pollfd> polled; ///< Description of polled sockets, including their descriptors.
    std::vector<Socket_info> peer_info; ///< Peer IP addresses and ports.
    std::queue<Socket_inbound_message> inbox; ///< Received messages queued for retrieval
    std::queue<Socket_info> connection_log; ///< Sockets that have been connected

    std::thread receiver_thread;

    /** Determines if server is monitoring its descriptors.
     * Used to stop the receiver thread when set to false.    
     */
    std::atomic<bool> is_running; 
    std::mutex inbox_mutex, peer_list_mutex, socket_mutex, allowed_list_mutex, connection_log_mutex;

    /**
     *  @brief Handle new connection request received by service (0th) socket.
     */
    void handle_request();

    /**
     *  @brief Receives a message from client socket and tries to queue it in inbox.
     *  @param id the client's id.
    */
    void handle_client(int id);
};



