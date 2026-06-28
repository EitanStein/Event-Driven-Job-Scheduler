#pragma once

#include <string>
#include <utility>
#include <cstring>

enum class CommStatus {Fail=false, Success=true};

struct MsgFormat{
    u_int32_t msg_size;
    std::string payload;

    MsgFormat(std::string&& payload) : msg_size(payload.size()), payload(payload) {}
};
