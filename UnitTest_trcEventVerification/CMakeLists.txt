cmake_minimum_required(VERSION 3.21.3)

# Set the name for the project
project(UnitTest_trcEventVerification)

# Configure compiler settings
set(CMAKE_C_STANDARD 99)

# Add executable with its required source files
add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/EventVerification_UnitTest.c
    ${RECORDER_DIR}/trcDiagnostics.c
    ${RECORDER_DIR}/trcEvent.c)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/config
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/mocks
	${RECORDER_DIR}/include
	${CMOCKA_DIR}/include
	${TEST_DIR}/utilities/EventVerification/include)

# Link CMocka with project
target_link_libraries(${PROJECT_NAME} cmocka)
