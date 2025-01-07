cmake_minimum_required(VERSION 3.21.3)

# Set the name for the project
project(UnitTest_RUNNER)

# Configure compiler settings
set(CMAKE_C_STANDARD 99)

# Add executable with its required source files
add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
