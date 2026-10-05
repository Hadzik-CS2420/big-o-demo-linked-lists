cmake_minimum_required(VERSION 3.24)
project(big-o-demo-linked-lists LANGUAGES CXX)

# Write build/compile_commands.json so C++ IntelliSense knows the real
# include paths and flags instead of guessing at them.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Time complexity demo
add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE REPO_DIR="${CMAKE_SOURCE_DIR}")

# Space complexity demo
add_executable(${PROJECT_NAME}-space src/space_main.cpp)
target_compile_definitions(${PROJECT_NAME}-space PRIVATE REPO_DIR="${CMAKE_SOURCE_DIR}")
