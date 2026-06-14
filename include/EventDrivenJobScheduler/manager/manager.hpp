#pragma once

#include <vector>
#include <deque>
#include <queue>
#include <EventDrivenJobScheduler/common/job.hpp>
#include <EventDrivenJobScheduler/utils/log_macros.hpp>
#include "worker.hpp"

#include <unistd.h>
#include <sys/wait.h>


struct JobQueue{
    std::queue<Job> asap_jobs;
    std::array<std::deque<Job>, static_cast<size_t>(Priority::NUM_PRIORITIES)> urgency_based_queues;

    [[nodiscard]] bool empty() const{
        if(!asap_jobs.empty())
            return false;

        for(const auto& job_queue : urgency_based_queues){
            if(!job_queue.empty())
                return false;
        }
        return true;
    }

    void push(Job&& job, Priority priority=Priority::LOW){
        if(priority == Priority::ASAP)
            asap_jobs.push(std::move(job));
        else
            urgency_based_queues[static_cast<size_t>(priority)].emplace_back(std::move(job));
    }

    [[nodiscard]] std::optional<Job> popAsapJob(Resource available_resources){
        LOG_INFO("JobQueue: there are pending asap jobs - attempting to pop the front one");
        std::optional<Job> result;
        Job& front_job = asap_jobs.front();

        if(front_job.resource_reqs <= available_resources){
            result.emplace(std::move(front_job));
            asap_jobs.pop();
        }
        else
            LOG_WARN("JobQueue: pop: front asap job requires more resources");
        
        return result;
    }

    [[nodiscard]] std::optional<Job> popRegularJob(Resource available_resources){
        LOG_INFO("JobQueue: attempting to execute the pop one of the most urgest non-asap jobs");
        std::optional<Job> result;
        for(auto& job_queue : urgency_based_queues){
            if(job_queue.empty())
                continue;

            for(auto it = job_queue.begin(); it != job_queue.end(); ++it){
                if(it->resource_reqs <= available_resources){
                    result.emplace(std::move(*it));
                    job_queue.erase(it);
                    LOG_INFO("JobQueue: pop: a job was selected");
                    break;
                }
            }
            break;
        }
        return result;
    }

    [[nodiscard]] std::optional<Job> pop(Resource available_resources){
        std::optional<Job> result;
        LOG_INFO("JobQueue: pop");
        if(!asap_jobs.empty())
            return popAsapJob(available_resources);
        else
            return popRegularJob(available_resources);
    }


    void UpdateJobPriorities(int timestamp){
        constexpr int time_diff = 60;
        for(size_t i = 1; i < urgency_based_queues.size(); ++i){
            std::deque<Job>& job_queue = urgency_based_queues[i];

            // TODO change timestamp check into its own function?
            while(!job_queue.empty() && job_queue.front().timestamp + time_diff*i > timestamp){
                // TODO this assumes each Job will only jump at most one priority up
                urgency_based_queues[i-1].emplace_back(std::move(job_queue.front()));
                job_queue.pop_front();
            }
        }
    }
};


class Manager{
    //std::vector<pid_t> active_workers; // TODO const limit? and maybe use array?
    JobQueue pending_jobs;
    Resource available_resource;

    [[nodiscard]] pid_t forkJob(Job&& job) const{
        pid_t pid = fork();
        if(pid < 0)
            LOG_ERROR("fork failed"); // TODO reinsert job? give job id?
        else if(pid == 0){
            // TODO maybe change state of job here so it overwrites CoW job memory
            // TODO think where to activate a worker here - right now it just forks from manager
            Worker worker(std::move(job.command), std::move(job.args_str));
            worker.execute();
        }
        
        return pid;
    }

public:
    void addJob(Job&& job, Priority priority=Priority::LOW) {
        pending_jobs.push(std::move(job), priority);
    }

    void addJob(std::string&& command, std::string&& args) {
        addJob(Job{std::forward<std::string>(command), std::forward<std::string>(args)});
    }

    [[nodiscard]] int giveJobToWorker(){
        LOG_INFO("assinging job to worker");

        if(pending_jobs.empty()){
            LOG_WARN("empty job queue");
            return -1;
        }

        pid_t pid = -1;
        if(std::optional<Job> job = pending_jobs.pop(available_resource))
            pid = forkJob(std::move(job.value()));

        if(pid < 0)
            return -1;
        
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
    
};