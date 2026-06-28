

add_executable(ClientApp main.cpp)


target_link_libraries(ClientApp
    PRIVATE
        logging_module
)


target_include_directories(ClientApp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
