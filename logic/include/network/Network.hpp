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
