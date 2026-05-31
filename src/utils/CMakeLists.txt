add_library(logging_module
    logger.cpp
)

target_link_libraries(logging_module
    PRIVATE
        project_options
    PUBLIC
        spdlog::spdlog
)

target_include_directories(logging_module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)