#pragma once

#include <vector>
#include <string>
#include <ranges>
#include <unistd.h>

class Job{
private:
    std::vector<std::string> command_args;

public:
    Job(std::string&& input){
        command_args = input |
                       std::views::split(' ') |
                       std::ranges::to<std::vector<std::string>>();
    }

    const std::vector<std::string>& getCommandArgs() const {return command_args;}

    void execute(){ // TODO move to worker (job should not execute on its own)
        char* args[command_args.size() + 1];

        for(size_t idx=0; auto& str_v : command_args){
            args[idx] = str_v.data();
            ++idx;
        }
        args[command_args.size()] = nullptr;

        execvp(args[0], args);
    }
};