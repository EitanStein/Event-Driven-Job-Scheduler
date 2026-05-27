#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>



TEST_CASE("Sanity check", "")
{
    REQUIRE(1==1);
}