#pragma once

#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include "EventDrivenJobScheduler/common/communication.hpp"
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Client{
private:
    pollfd fd;
    sockaddr_in address{}; // TODO change to input arguments or ENV variables
public:
    Client() : fd(socket(AF_INET, SOCK_STREAM, 0)) {
        address.sin_family = AF_INET;
        address.sin_port = htons(8080);
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    }

    ~Client() {
        close(fd.fd);
    }

    [[nodiscard]] CommStatus connectToManager(){
        if(connect(fd.fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0){
            LOG_ERROR("Client failed to connect to manager");
            return CommStatus::Fail;
        }
            
        LOG_INFO("Client connected to manager");
        return CommStatus::Success;
    }

    [[nodiscard]] CommStatus sendCommand(MsgFormat&& msg){
        auto to_send_msg_size =  htonl(msg.msg_size);
        if(write(fd.fd, &to_send_msg_size, sizeof(msg.msg_size)) < 0){
            LOG_ERROR("Client failed to send message");
            return CommStatus::Fail;
        }
        if(write(fd.fd, msg.payload.data(), msg.msg_size) < 0){
            LOG_ERROR("Client failed to send message");
            return CommStatus::Fail;
        }

        LOG_INFO("Client sent a message");
        return CommStatus::Success;
    }
};