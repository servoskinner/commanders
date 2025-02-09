#pragma once

#include "Misc_functions.hpp"
#include "Network.hpp"

class TCP_server
{
    public:
    std::set<in_addr_t> allowed_ips; ///< IP addresses that can connect to the server.

    /** If set to true, accepts all connection requests
     *  without checking sender IP against allowed_ips.
     */
    bool accept_all = false;
    //* sends a message after establishing connection to client.
    bool do_handshake = false;

    /**
     *  @brief Constructor of TCP_server.
     *  @param port port to host the service socket used to request new connections.
     */
    TCP_server(u_short port);
    ~TCP_server();

    /**
     *  @brief Ran by dedicated thread: checks for incoming messages on open sockets and processes them.
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
     *  @return A set of IPs that are allowed to connect.
     */
    const std::set<in_addr_t> get_allowed_ips() { return allowed_ips;}
    
    /**
     *  @return The port that handles connection requests.
     */
    const u_short get_port() { return own_port;}

    /**
     *  @brief Pops first value from queue that holds Socket_infos of recently connected peers.
     */
    const std::optional<Socket_info> get_connection_event();
    
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
