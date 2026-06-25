#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include "EventDrivenJobScheduler/manager/manager.hpp"


TEST_CASE("Empty job queue", ""){
    INIT_LOGGER();
    Manager manager;
    REQUIRE(!manager.giveJobToWorker());
}

TEST_CASE("basic job", "")
{
    INIT_LOGGER();
    Manager manager;
    manager.addJob("sleep", "1");
    
    REQUIRE(manager.giveJobToWorker());
}

TEST_CASE("multiple jobs", "")
{
    INIT_LOGGER();
    Manager manager;
    manager.addJob("echo", "1");
    manager.addJob("ls", "--version");
    manager.addJob("ls", "-la");

    REQUIRE(manager.giveJobToWorker());
    REQUIRE(manager.giveJobToWorker());
}

TEST_CASE("multiple jobs with final invalid job", "")
{
    INIT_LOGGER();
    Manager manager;
    manager.addJob("echo", "1");
    manager.addJob("ls", "--version");
    manager.addJob("IDontExistProbably", "45 45");

    REQUIRE(manager.giveJobToWorker());
    REQUIRE(manager.giveJobToWorker());
    REQUIRE(manager.giveJobToWorker());
}
