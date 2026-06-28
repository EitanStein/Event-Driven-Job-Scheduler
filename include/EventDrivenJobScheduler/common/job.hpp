#pragma once

#include <vector>
#include <string>
#include <ranges>
#include "resource.hpp"
#include "EventDrivenJobScheduler/utils/log_macros.hpp"

enum class Priority{HIGH=0, MED, LOW, NUM_PRIORITIES, ASAP=0};

struct Job{
    std::string command; 
    std::string args_str;
    Resource resource_reqs;
    // TODO proper time stamp
    int timestamp{};

    // TODO priority - based on either client input or life length

    Job(std::string&& command, 
        std::string&& args_str, 
        Resource resources,
        int timestamp=0) :  
            command(std::move(command)),
            args_str(std::move(args_str)),
            resource_reqs(resources),
            timestamp(timestamp)
    {}
    Job(std::string&& command, 
        std::string&& args_str) : 
            command(std::move(command)),
            args_str(std::move(args_str)),
            resource_reqs() 
    {}
    Job(std::string&& input, int timestamp=0) : 
            command(""), 
            args_str(""), 
            resource_reqs(), 
            timestamp(timestamp)
    {
        auto input_parts = input |
            std::views::split(' ') |
            std::ranges::to<std::vector<std::string>>() |
            std::views::filter([](std::string& str){ return str.size() > 0;});

        enum class Option {exec, args, mem, cpu, none};
        Option next_val = Option::none;
        for(auto& input_part : input_parts){
            if(input_part == "")
                continue;
            switch(next_val){
                case Option::none:
                    if(input_part == "--exec")
                        next_val = Option::exec;
                    else if(input_part == "--args")
                        next_val = Option::args;
                    else if(input_part == "--mem")
                        next_val = Option::mem;
                    else if(input_part == "--cpu")
                        next_val = Option::cpu;
                    else
                        LOG_ERROR("unfamiliar argument: {}", input_part);
                    continue;

                case Option::exec:
                    command = std::move(input_part);
                    break;
                case Option::args:
                    args_str = std::move(input_part);
                    break;
                case Option::mem:
                    resource_reqs.memory = std::stoi(input_part);
                    break;
                case Option::cpu:
                    resource_reqs.cpu = std::stoi(input_part);
                    break;
            }
            next_val = Option::none;
            
        }
    }
    
    // TODO handle single command string and turn it into multiple values based on flags
    // --exec, --args, --mem, --cpu
    // Job(std::string&& input): input_str(str) {
    //     std::vector<std::string> input_parts = input |
    //                                             std::views::split(' ') |
    //                                             std::ranges::to<std::vector<std::string>>();
    // }
};