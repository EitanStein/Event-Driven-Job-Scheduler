cmake_minimum_required(VERSION 3.28) 
project(EventDrivenJobScheduler LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerSettings)
create_project_options()


include(FetchContent)
if(TRACY_ENABLE)
  FetchContent_Declare(
      tracy
      GIT_REPOSITORY https://github.com/wolfpld/tracy.git
      GIT_TAG        c556831ddc6fe26d2fce01c14c97205a9dad46d5 #0.12.2
      FIND_PACKAGE_ARGS CONFIG
  )
  FetchContent_MakeAvailable(tracy)
endif()

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        486b55554f11c9cccc913e11a87085b2a91f706f #1.16.0
    FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(spdlog)

include(CTest)
enable_testing()
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        6ee0826dcae55ed1e06b2c5701981221e979e1e6 #3.15.0
    FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)
add_subdirectory(tests)
