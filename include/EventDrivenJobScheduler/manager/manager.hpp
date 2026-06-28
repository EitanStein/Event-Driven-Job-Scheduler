#pragma once

#include <vector>
#include <deque>
#include <queue>
#include <unordered_set>

#include <EventDrivenJobScheduler/common/job.hpp>
#include <EventDrivenJobScheduler/utils/log_macros.hpp>
#include "worker.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/signalfd.h>
#include <poll.h>

#include <netinet/in.h>
#include <sys/socket.h>

struct Doorbell{
    pollfd fd;
    sockaddr_in address{};

    Doorbell() : fd(socket(AF_INET, SOCK_STREAM, 0)), address({}){
        address.sin_family = AF_INET;
        address.sin_port = htons(8080);
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
};

constexpr int CLIENT_BACKLOG = 10;
typedef std::string JobMsg;
// TODO check std::signal
struct SingalHandler{
    enum Signal {Terminate, Worker, Client, Ignore};
    sockaddr_in doorbell_address{};
    std::vector<pollfd> fd_vec{};

    SingalHandler() {
        sigset_t mask_for_wroker_fd;
        sigemptyset(&mask_for_wroker_fd);
        sigaddset(&mask_for_wroker_fd, SIGCHLD);
        sigaddset(&mask_for_wroker_fd, SIGINT);
        sigaddset(&mask_for_wroker_fd, SIGTERM);

        if(sigprocmask(SIG_BLOCK, &mask_for_wroker_fd, nullptr) == -1)
            throw std::runtime_error("SingalHandler: sigprocmask error");
        
        fd_vec.emplace_back();
        fd_vec[0].fd = signalfd(-1, &mask_for_wroker_fd, SFD_CLOEXEC);
        fd_vec[0].events = POLLIN;
        if(fd_vec[0].fd == -1){
            throw std::runtime_error("SingalHandler: signalfd creation failed");
        }

        fd_vec.emplace_back();
        fd_vec[1].fd = socket(AF_INET, SOCK_STREAM, 0);
        fd_vec[1].events = POLLIN;
        doorbell_address.sin_family = AF_INET;
        doorbell_address.sin_port = htons(8080);
        doorbell_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if(bind(fd_vec[1].fd, reinterpret_cast<struct sockaddr*>(&doorbell_address), (socklen_t)sizeof(doorbell_address)) < 0){
            close(fd_vec[0].fd);
            throw std::runtime_error("SingalHandler: doorbell bind failed");
        }

        if(listen(fd_vec[1].fd, CLIENT_BACKLOG) < 0){
            close(fd_vec[0].fd);
            close(fd_vec[1].fd);
            throw std::runtime_error("SingalHandler: listen failed");
        }
            
    }

    ~SingalHandler(){
        for(auto fd : fd_vec){
            close(fd.fd);
        }
    }

    void acceptNewClient(){
        size_t address_len = sizeof(doorbell_address);
        int new_client_fd = accept(fd_vec[1].fd, 
                                    reinterpret_cast<struct sockaddr*>(&doorbell_address),
                                    (socklen_t*)&address_len);

        if(new_client_fd < 0)
            LOG_ERROR("waitForSignal: accept: failed to connect to new client request");
        else{
            LOG_INFO("new client fd received: {}", new_client_fd);
            fd_vec.emplace_back();
            fd_vec.back().fd = new_client_fd;
            fd_vec.back().events = POLLIN;
        }
    }

    [[nodiscard]] Signal getWorkerSignal(){
        signalfd_siginfo siginfo;

        ssize_t size = read(fd_vec[0].fd, &siginfo, sizeof(siginfo));

        if(siginfo.ssi_signo == SIGCHLD){
            LOG_INFO("child signal received");
            return Signal::Worker;
        }
        else if (siginfo.ssi_signo == SIGINT || siginfo.ssi_signo == SIGTERM){
            LOG_INFO("Termination signal received");
            return Signal::Terminate;
        }

        LOG_ERROR("worker signal recieved was an unknown signal {}", siginfo.ssi_signo);
        return Signal::Worker;
    }

    [[nodiscard]] JobMsg getJobData(int fd, u_int32_t msg_size){
        std::vector<char> buffer(msg_size);
        LOG_DEBUG("manager receiving job msg of size: {}", msg_size);
        read(fd, buffer.data(), msg_size);
        LOG_DEBUG("manager received job info: {}", std::string{buffer.data()});

        return std::string{buffer.data()};
    }

    [[nodiscard]] std::pair<Signal, std::string> waitForSignal(){
        LOG_DEBUG("Polling...");
        int ready = poll(fd_vec.data(), fd_vec.size(), -1);
        LOG_DEBUG("Poll successful");

        if(fd_vec[0].revents && POLLIN){
            // worker finished or received temination signal
            std::pair<Signal, std::string> result{getWorkerSignal(), ""};
            return result;
            
        }
        else if(fd_vec[1].revents && POLLIN){
            acceptNewClient();
        }
        else{
            int fd_idx=2;
            while(fd_idx < fd_vec.size()){
                if(fd_vec[fd_idx].revents && POLLIN)
                    break;
                ++fd_idx;
            }
            if(fd_idx == fd_vec.size()){
                LOG_ERROR("poll returned a singal but didnt match any fd event");
                return {Signal::Ignore, ""};
            }

            u_int32_t msg_size = 0;
            ssize_t size = read(fd_vec[fd_idx].fd, &msg_size, sizeof(msg_size));
            if(size == 0){
                LOG_INFO("close connection to client with fd {}", fd_vec[fd_idx].fd);
                close(fd_vec[fd_idx].fd);
                fd_vec.erase(fd_vec.begin() + fd_idx);
            }
            else{
                std::pair<Signal, std::string> result{Signal::Client, getJobData(fd_vec[fd_idx].fd, ntohl(msg_size))};
                return result;
            }   
        }

        return {Signal::Ignore, ""};
    }
};


struct JobQueue{
    std::queue<Job> asap_jobs{};
    std::array<std::deque<Job>, static_cast<size_t>(Priority::NUM_PRIORITIES)> urgency_based_queues{};

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
    Resource available_resource{};
    std::unordered_map<pid_t, Resource> active_workers{};
    JobQueue pending_jobs{};
    SingalHandler sig_handler{};

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
    Manager() {}
    Manager(Resource max_resources) : available_resource(max_resources) {}

    ~Manager() {
        for(auto worker : active_workers){
            kill(worker.first, SIGTERM);
        }

        int status;
        pid_t pid = 0;

        while((pid = waitpid(-1, &status, WNOHANG)) > 0){
            handleFinishedWorker(pid);
        }

        for(auto worker : active_workers){
            kill(worker.first, SIGKILL);
            waitpid(worker.first, nullptr, 0);
        }
    }
    

    // TODO what happens if a job is added that requires more resources than the manager can afford?
    void addJob(Job&& job, Priority priority=Priority::LOW) {
        pending_jobs.push(std::move(job), priority);
    }

    void addJob(std::string&& command, std::string&& args) {
        addJob(Job{std::forward<std::string>(command), std::forward<std::string>(args)});
    }

    [[nodiscard]] bool giveJobToWorker(){
        LOG_DEBUG("assinging job to worker");

        if(pending_jobs.empty()){
            LOG_DEBUG("giveJobToWorker: empty job queue");
            return false;
        }

        if(std::optional<Job> job = pending_jobs.pop(available_resource)){
            Resource reqs = job.value().resource_reqs;
            available_resource -= job.value().resource_reqs;
            // TODO if forks fail we lose the job - change to front + pop like a queue
            pid_t pid = forkJob(std::move(job.value()));
            if(pid <= 0)
                return false;
            LOG_INFO("forked process child {}", pid);
            // TODO check if it needs std::in_place or something else for in place construction
            // not a big deal in this case but its a good concept to know
            active_workers[pid] = reqs;
            return true;
        }
        
        LOG_INFO("giveJobToWorker: not enough resources");
        return false;
    }

    void handleFinishedWorker(pid_t pid){
        LOG_INFO("process child {} finished", pid);
        if(active_workers.find(pid) == active_workers.end()){
            LOG_DEBUG("cant find {}", pid);
            return;
        }

        available_resource += active_workers.at(pid);
        active_workers.erase(pid);
    }

    [[nodiscard]] SingalHandler::Signal handleSignal(){
        auto signal = sig_handler.waitForSignal();

        if(signal.first == SingalHandler::Signal::Worker){
            int status;
            pid_t pid = 0;
            while((pid = waitpid(-1, &status, WNOHANG)) > 0){
                handleFinishedWorker(pid);
            }
        }
        else if(signal.first == SingalHandler::Signal::Client){
            addJob(Job(std::move(signal.second)));
        }
        
        return signal.first;
    }

    void mainLoop(){
        LOG_DEBUG("starting main loop");
        while(true){ 
            LOG_DEBUG("waiting for signal");
            SingalHandler::Signal signal = handleSignal();
            LOG_DEBUG("signal handled");
            if(signal == SingalHandler::Signal::Terminate)
                break;
            else if(signal == SingalHandler::Signal::Ignore)
                continue;
            
            // send new jobs to children
            while(giveJobToWorker()) {}
        }
        
    }
    
};