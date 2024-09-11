#pragma once

#include "Misc_functions.hpp"
#include "Network.hpp"

class TCP_client
{
    public:
    TCP_client();
    ~TCP_client();

    /** Duration for which the thread will wait for 
     *  confirmation message after connecting.
     *  If set to 0 or less, the connection is assumed to be
     *  established in any case.
     */
    int handshake_timeout_ms = 0;
    /**
     *  @brief Retrieves a message from inbox queue.
     *  @returns Message paired with sender's address.
     */
    Socket_inbound_message get_message();

    /**
     *  @brief Ran by dedicated thread: checks for incoming messages on open sockets and processes them.
     *  @return Whether all bytes were delivered successfully.
     */
    bool send_msg(const std::vector<char>& msg);

    /**
     *  @brief Attempts to connect to server with given address
     *  @return Whether connection attempt was successful.
     */
    bool connect_to(const Socket_info& destination, int n_attempts = 1, int ms_between = 0);

    /**
     *  @brief Severs connection with the server.
     *  @return Whether the connection was active and was severed successfully.
     */
    bool disconnect();
    const std::optional<Socket_info> get_connection();

    private:
    void receive_messages();

    u_short own_port = -1;
    std::atomic<int> socket_fdesc = -1;

    std::thread receiver_thread;
    std::atomic<bool> is_polling;
    std::queue<Socket_inbound_message> message_queue;
    std::optional<Socket_info> server = {};
    std::mutex receiver_mutex, confirmation_mutex;
};