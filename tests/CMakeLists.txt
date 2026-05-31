

add_executable(test_jobs common/test_job.cpp)
add_executable(test_manager manager/test_manager.cpp)
add_executable(test_worker manager/test_worker.cpp)

target_link_libraries(test_jobs
    PRIVATE
        spdlog::spdlog
        Catch2::Catch2WithMain
)

target_include_directories(test_jobs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(test_manager
    PRIVATE
        spdlog::spdlog
        Catch2::Catch2WithMain
)

target_include_directories(test_manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(test_worker
    PRIVATE
        spdlog::spdlog
        Catch2::Catch2WithMain
)

target_include_directories(test_worker
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME Testing_jobs COMMAND $<TARGET_FILE:test_jobs>)
add_test(NAME Testing_manager COMMAND $<TARGET_FILE:test_manager>)
add_test(NAME Testing_worker COMMAND $<TARGET_FILE:test_worker>)