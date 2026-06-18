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
    // REQUIRE(manager.giveJobToWorker());
}


TEST_CASE("main loop no jobs", "")
{
    INIT_LOGGER();
    Manager manager;
    std::vector<Job> no_jobs{};
    manager.main_loop(no_jobs);

    REQUIRE(1==1);
}


TEST_CASE("main loop has to wait for resources", "")
{
    INIT_LOGGER();
    Manager manager{Resource{100, 100}};;
    std::vector<Job> jobs{Job{"sleep", "1", Resource{70, 70}},
                          Job{"sleep", "1", Resource{90, 20}},  
                          Job{"sleep", "1", Resource{20, 90}}
                        };
    manager.main_loop(jobs);

    REQUIRE(1==1);
}


TEST_CASE("main loop no need to wait for resources", "")
{
    INIT_LOGGER();
    Manager manager{Resource{100, 100}};;
    std::vector<Job> jobs{Job{"sleep", "1", Resource{10, 20}},
                          Job{"sleep", "1", Resource{40, 30}},  
                          Job{"sleep", "1", Resource{40, 40}}
                        };
    manager.main_loop(jobs);

    REQUIRE(1==1);
}