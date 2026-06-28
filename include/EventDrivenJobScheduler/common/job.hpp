#pragma once

#include <vector>
#include <string>
#include <ranges>
#include "resource.hpp"
#include "EventDrivenJobScheduler/utils/log_macros.hpp"

enum class Priority{HIGH=0, MED, LOW, NUM_PRIORITIES, ASAP};

struct Job{
    enum Option {exec, args, mem, cpu, none};
private:
    Option getOption(std::string_view str){
        if(str == "--exec")
            return Option::exec;
        else if(str == "--args")
            return Option::args;
        else if(str == "--mem")
            return Option::mem;
        else if(str == "--cpu")
            return Option::cpu;
        else
            LOG_ERROR("unfamiliar argument: {}", str);
        return Option::none;
    }

    void fillJobInfo(std::string&& val, Option option){
        switch(option){
            case Option::exec:
                command = std::move(val);
                break;
            case Option::args:
                args_str = std::move(val);
                break;
            case Option::mem:
                resource_reqs.memory = std::stoi(val);
                break;
            case Option::cpu:
                resource_reqs.cpu = std::stoi(val);
                break;
        }
    }
public:
    

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
                        std::views::filter([](auto&& str){ return !std::ranges::empty(str);});
            

        
        Option next_val = Option::none;
        for(auto&& input_token : input_parts){
            if(next_val == Option::none){
                std::string_view input_view(&*input_token.begin(), std::ranges::distance(input_token));
                next_val = getOption(input_view);
            }
            else{
                fillJobInfo(std::string{input_token.begin(), input_token.end()}, next_val);
                next_val = Option::none;
            }
        }
    }

};