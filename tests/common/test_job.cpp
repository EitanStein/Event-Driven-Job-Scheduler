#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <unistd.h>
#include <sys/wait.h>

#include "EventDrivenJobScheduler/common/job.hpp"


TEST_CASE("command split test", "")
{
    Job job("ls --version");
    auto args = job.getCommandArgs();
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "ls");
    REQUIRE(args[1] == "--version");
}

int fork_job(std::string&& input){
    pid_t pid = fork();
    if(pid < 0)
        throw std::runtime_error("fork failed");
    
    if (pid == 0) {
        Job job(std::forward<std::string>(input));
        job.execute();
        
        exit(127); 
    } else {
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
}

TEST_CASE("basic command execution test using fork", ""){

    int status = fork_job("sleep 1");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = fork_job("ls --version");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = fork_job("ls  --version");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = fork_job("ls  --version  ");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);
}