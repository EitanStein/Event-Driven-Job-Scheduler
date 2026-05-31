#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include "EventDrivenJobScheduler/manager/manager.hpp"

TEST_CASE("Empty job queue", ""){
    INIT_LOGGER();
    Manager manager;
    int status = manager.giveJobToWorker();
    REQUIRE(status == -1);
}

TEST_CASE("basic job", "")
{
    INIT_LOGGER();
    Manager manager;
    manager.addJob("sleep", "1");
    
    int status = manager.giveJobToWorker();
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);
}

TEST_CASE("multiple jobs", "")
{
    INIT_LOGGER();
    Manager manager;
    manager.addJob("echo", "1");
    manager.addJob("ls", "--version");
    manager.addJob("ls", "-la");

    int status = manager.giveJobToWorker();
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = manager.giveJobToWorker();
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);
}

TEST_CASE("multiple jobs with final invalid job", "")
{
    INIT_LOGGER();
    Manager manager;
    manager.addJob("echo", "1");
    manager.addJob("ls", "--version");
    manager.addJob("IDontExistProbably", "45 45");

    int status = manager.giveJobToWorker();
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = manager.giveJobToWorker();
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) == 0);

    status = manager.giveJobToWorker();
    REQUIRE(WIFEXITED(status));
    REQUIRE(WEXITSTATUS(status) != 0);
}