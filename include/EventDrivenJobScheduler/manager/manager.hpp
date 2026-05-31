#pragma once

#include <vector>
#include <queue>
#include <EventDrivenJobScheduler/common/job.hpp>
#include "worker.hpp"
#include <print> // TODO replace with logger

#include <unistd.h>
#include <sys/wait.h>

class Manager{
    //std::vector<pid_t> active_workers; // TODO const limit? and maybe use array?
    std::queue<Job> pending_jobs;
public:
    Manager() {}

    void addJob(Job&& job) {
        pending_jobs.push(std::move(job));
    }

    void addJob(std::string&& command, std::string&& args) {
        pending_jobs.emplace(std::forward<std::string>(command), 
                             std::forward<std::string>(args));
    }

    int giveJobToWorker(){
        if(pending_jobs.empty())
            return -1; // TODO return error value or throw error

        Job job = std::move(pending_jobs.front());
        pending_jobs.pop();

        pid_t pid = fork();
        if(pid < 0)
            std::println("error with fork");
        else if(pid == 0){
            // TODO maybe change state of job here so it overwrites CoW job memory
            // TODO think where to activate a worker here - right now it just forks from manager
            Worker worker(std::move(job.command), std::move(job.args_str));
            worker.execute();
        }
        else{
            int status;
            waitpid(pid, &status, 0);
            return status;
        }

        return -1;
    }
    
};