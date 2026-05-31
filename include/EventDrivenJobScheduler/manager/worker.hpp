#pragma once

#include <string>
#include <vector>
#include <ranges>
#include <unistd.h>
#include <EventDrivenJobScheduler/utils/log_macros.hpp>


class Worker{
private:
    std::string command;
    std::vector<std::string> command_args;
public:
    Worker(std::string&& command, std::string&& args) : command(std::move(command)) {
        command_args = args |
                       std::views::split(' ') |
                       std::ranges::to<std::vector<std::string>>();
    }

    [[nodiscard]] const std::string& getCommand() const {return command;}
    [[nodiscard]] const std::vector<std::string>& getCommandArgs() const {return command_args;}
    
    void execute(){
        LOG_INFO("executing command {}", command); // TODO add args in log?
        char* args[command_args.size() + 2];

        args[0] = command.data();
        for(size_t idx=1; auto& str_v : command_args){
            args[idx] = str_v.data();
            ++idx;
        }
        args[command_args.size() + 1] = nullptr;

        execvp(args[0], args);
    }
};