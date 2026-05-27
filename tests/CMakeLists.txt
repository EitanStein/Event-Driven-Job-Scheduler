

add_executable(tests test_main.cpp)

target_link_libraries(tests
    PRIVATE
        spdlog::spdlog
        Catch2::Catch2WithMain
)

add_test(NAME Testing COMMAND $<TARGET_FILE:tests>)