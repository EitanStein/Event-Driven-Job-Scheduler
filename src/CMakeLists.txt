

add_executable(JobScheduler main.cpp)

target_link_libraries(JobScheduler
    PRIVATE
        spdlog::spdlog
        Catch2::Catch2WithMain
)

add_test(NAME MIS_Graph_Checks COMMAND $<TARGET_FILE:tests>)