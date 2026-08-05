cmake_minimum_required(VERSION 3.0)
project(petmpc VERSION 1.0.0 LANGUAGES CXX)

# major config
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(APP_NAME "petmpc")

# set compiler flags 
set(COMPILE_OPTIONS "-Wall" "-pedantic" "-g3" "-O0" "-std=c++20" "-DDEBUG")

# set c++20 required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main
)
FetchContent_MakeAvailable(argparse)
FetchContent_MakeAvailable(ftxui)

# sources
file(GLOB SOURCES "*.cpp" "ui/*.cpp")

# release executable
add_executable(${APP_NAME} ${SOURCES})
target_link_libraries(${APP_NAME} argparse ao mpg123 ftxui::ftxui)
target_compile_options(${APP_NAME} PRIVATE ${COMPILE_OPTIONS})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
