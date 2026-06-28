

add_executable(ManagerApp main.cpp)


target_link_libraries(ManagerApp
    PRIVATE
        logging_module
)


target_include_directories(ManagerApp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
