#pragma once

#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include "EventDrivenJobScheduler/common/communication.hpp"
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Client{
    pollfd fd;
    sockaddr_in address{}; // TODO change to input arguments or ENV variables

    Client() : fd(socket(AF_INET, SOCK_STREAM, 0)) {
        address.sin_family = AF_INET;
        address.sin_port = htons(8080);
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    }

    ~Client() {
        close(fd.fd);
    }

    int Connect(){
        int result = connect(fd.fd, reinterpret_cast<sockaddr*>(&address), sizeof(address));
        if(result < 0)
            LOG_ERROR("Client failed to connect to manager");

        return result;
    }

    int SendCommand(MsgFormat&& msg){
        if(write(fd.fd, msg.GetMsg().data(), msg.GetTotSize()) < 0)
            LOG_ERROR("failed to send message");
    }

    
};