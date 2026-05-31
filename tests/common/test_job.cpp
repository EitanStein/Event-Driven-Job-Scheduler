#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <unistd.h>
#include <sys/wait.h>

#include "EventDrivenJobScheduler/common/job.hpp"

// TODO add test for correct init after proper construction function is made
TEST_CASE("TODO add prper test for job", ""){
    REQUIRE(1==1);
}
