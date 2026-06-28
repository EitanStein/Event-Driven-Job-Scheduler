#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include "EventDrivenJobScheduler/client/client.hpp"


int main(){
    INIT_LOGGER("client_logs.txt");
    Client client;
    auto result = client.connectToManager();
    sleep(1);
    result = client.sendCommand(MsgFormat{"--exec sleep --args 1"});
    result = client.sendCommand(MsgFormat{"--exec echo --args 1"});

    return 0;
}