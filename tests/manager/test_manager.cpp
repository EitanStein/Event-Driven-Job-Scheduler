#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include "EventDrivenJobScheduler/manager/manager.hpp"
#include "EventDrivenJobScheduler/client/client.hpp"


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


TEST_CASE("managing job from client", "")
{
    INIT_LOGGER();
    Manager manager;
    int pid = fork();

    if(pid > 0){
        Client client;
        sleep(1);
        REQUIRE(client.connectToManager() == CommStatus::Success);
        REQUIRE(client.sendCommand(MsgFormat{"--exec sleep --args 1"}) == CommStatus::Success);
        REQUIRE(client.sendCommand(MsgFormat{"--exec echo --args 1"}) == CommStatus::Success);
        _exit(0);
    }
    else if(pid == 0){
        REQUIRE(manager.handleSignal() == SignalHandler::Signal::Ignore);
        SignalHandler::Signal result = manager.handleSignal();
        REQUIRE((result == SignalHandler::Signal::Ignore || result == SignalHandler::Signal::Worker));
        result = manager.handleSignal();
        REQUIRE((result == SignalHandler::Signal::Ignore || result == SignalHandler::Signal::Worker));
        result = manager.handleSignal();
        REQUIRE((result == SignalHandler::Signal::Ignore || result == SignalHandler::Signal::Worker));
        result = manager.handleSignal();
        REQUIRE((result == SignalHandler::Signal::Ignore || result == SignalHandler::Signal::Worker));
    }

}