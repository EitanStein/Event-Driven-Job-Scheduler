#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <unistd.h>
#include <sys/wait.h>

#include "EventDrivenJobScheduler/common/job.hpp"


TEST_CASE("job creation using command line", ""){
    Job job("--exec sleep --args 1 --mem 3 --cpu 4");
    REQUIRE(job.command == "sleep");
    REQUIRE(job.args_str == "1");
    REQUIRE(job.resource_reqs.memory == 3);
    REQUIRE(job.resource_reqs.cpu == 4);
}

TEST_CASE("job creation using command line with excess spaces", ""){
    Job job("--exec   sleep --args       1");
    REQUIRE(job.command == "sleep");
    REQUIRE(job.args_str == "1");
    REQUIRE(job.resource_reqs.memory == 0);
    REQUIRE(job.resource_reqs.cpu == 0);
}


TEST_CASE("job creation using unordered command", ""){
    Job job("--args 1 --mem 3 --exec   sleep --cpu       1");
    REQUIRE(job.command == "sleep");
    REQUIRE(job.args_str == "1");
    REQUIRE(job.resource_reqs.memory == 3);
    REQUIRE(job.resource_reqs.cpu == 1);
}