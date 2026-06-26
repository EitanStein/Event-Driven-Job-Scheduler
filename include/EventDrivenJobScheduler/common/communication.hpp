#pragma once

#include <string>
#include <utility>
#include <cstring>

struct MsgFormat{
    int32_t msg_size;
    std::string payload;

    MsgFormat(std::string&& payload) : msg_size(payload.size()), payload(payload) {}

    std::string GetMsg() {
        char str_msg_size[4];
        std::memcpy(str_msg_size, static_cast<void*>(&msg_size), 4);

        return std::string(str_msg_size) + payload;
    }

    size_t GetTotSize() const {
        return payload.size() + sizeof(msg_size);
    }
};
