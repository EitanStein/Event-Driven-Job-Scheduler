#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include <unistd.h>
#include <sys/wait.h>

#include "EventDrivenJobScheduler/manager/worker.hpp"


TEST_CASE("command split test", "")
{
    INIT_LOGGER();
    Worker worker("ls", "-l -a");
    REQUIRE(worker.getCommand() == "ls");
    REQUIRE(worker.getCommandArgs().size() == 2);
    REQUIRE(worker.getCommandArgs()[0] == "-l");
    REQUIRE(worker.getCommandArgs()[1] == "-a");
}

int fork_job(std::string&& command, std::string&& args){
    pid_t pid = fork();
    if(pid < 0)
        throw std::runtime_error("fork failed");
    
    if (pid == 0) {
        Worker worker(std::forward<std::string>(command), std::forward<std::string>(args));
        worker.execute();
        
        exit(127); 
    } else {
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
}


TEST_CASE("basic command execution test using fork", ""){
    INIT_LOGGER();
    int status = fork_job("sleep", "1");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = fork_job("ls", "--version");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = fork_job("ls", "  --version");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = fork_job("ls", " --version  ");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = fork_job("IDontExistProabably", " --45  ");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) != 0);

    status = fork_job("ls", " --vermillion  ");
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) != 0);
}