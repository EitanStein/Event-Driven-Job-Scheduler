

add_executable(ManagerApp main.cpp)

target_link_libraries(ManagerApp
    PRIVATE
        spdlog::spdlog
        Catch2::Catch2WithMain
)
