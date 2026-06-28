#include "EventDrivenJobScheduler/utils/log_macros.hpp"
#include "EventDrivenJobScheduler/manager/manager.hpp"


int main(){
    INIT_LOGGER("manager_logs.txt");
    Manager manager(Resource{100, 100});
    manager.mainLoop();
    return 0;
}