#pragma once
#include <functional>
#include "socket_utils.h"
#include "logging.h"
namespace Common{

    constexpr size_t TCPBufferSize = 64*1024*1024;
    struct TCPSocket{

        int fd_ = -1;
        char *send_buffer_ = nullptr;
        size_t next_send_valid_index_ = 0;
        char *rcv_buffer_ = nullptr;
        size_t next_rcv_valid_index_ = 0;
        bool send_disconnected_ = false;
        bool recv_disconnected_ = false;
        struct sockaddr_in inInAddr;
        std::function<void(TCPSocket *s, Nanos rx_time)> recv_callback_;
        std::string time_str_;
        Logger &logger_;
        

        explicit TCPSocket(Logger &logger):logger_(logger){
            send_buffer_  = new char[TCPBufferSize];
            rcv_buffer_ = new char[TCPBufferSize];
            
        }


        auto destroy() noexcept -> void;

        auto connect(const std::string &ip, const std::string &iface, int port, bool is_listening) -> int;

        auto send(const void *data, size_t len) noexcept -> void;

        auto sendAndRecv() noexcept -> bool;
        ~TCPSocket(){
            destroy();
            delete[] send_buffer_; send_buffer_ = nullptr;
            delete[] rcv_buffer_; rcv_buffer_ = nullptr;
        }

        TCPSocket() = delete;
        TCPSocket(const TCPSocket &) = delete;
        TCPSocket(const TCPSocket &&)  = delete;
        TCPSocket& operator=(const TCPSocket &) = delete;
        TCPSocket& operator=(const TCPSocket &&) = delete;

    };

}