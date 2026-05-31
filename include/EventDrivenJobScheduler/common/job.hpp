#pragma once

#include <vector>
#include <string>
#include <ranges>
#include "resource.hpp"

struct Job{
    size_t id;
    std::string command; 
    std::string args_str;
    Resource resource_reqs;
    // TODO priority - based on either client input or life length

    Job(std::string&& command, 
        std::string&& args_str, 
        Resource resources) : 
            id(0), 
            command(std::move(command)),
            args_str(std::move(args_str)),
            resource_reqs(std::move(resources)) 
    {}
    Job(std::string&& command, 
        std::string&& args_str) : 
            id(0), 
            command(std::move(command)),
            args_str(std::move(args_str)),
            resource_reqs({}) 
    {}
    
    // TODO handle single command string and turn it into multiple values based on flags
    // --exec, --args, --mem, --cpu
    // Job(std::string&& input): input_str(str) {
    //     std::vector<std::string> input_parts = input |
    //                                             std::views::split(' ') |
    //                                             std::ranges::to<std::vector<std::string>>();
    // }
};