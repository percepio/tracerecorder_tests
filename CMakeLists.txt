cmake_minimum_required(VERSION 3.21.3)

# Set name of project
project(UnitTest_All)

# Set global variables
set(RECORDER_DIR ../../TraceRecorderSource)
set(TEST_DIR ../../../../test)
set(CMOCKA_DIR ../cmocka)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# List subprojects
add_subdirectory(cmocka)

add_subdirectory(UnitTest_trcCounter)
add_subdirectory(UnitTest_trcDependency)
add_subdirectory(UnitTest_trcDiagnostics)
add_subdirectory(UnitTest_trcEntryTable)
add_subdirectory(UnitTest_trcError)
add_subdirectory(UnitTest_trcEvent)
add_subdirectory(UnitTest_trcEventBuffer)
add_subdirectory(UnitTest_trcExtension)
add_subdirectory(UnitTest_trcHeap)
#add_subdirectory(UnitTest_trcInternalEventBuffer)
add_subdirectory(UnitTest_trcInterval)
add_subdirectory(UnitTest_trcISR)
add_subdirectory(UnitTest_trcMultiCoreEventBuffer)
add_subdirectory(UnitTest_trcObject)
add_subdirectory(UnitTest_trcPrint)
add_subdirectory(UnitTest_trcRunnable)
add_subdirectory(UnitTest_trcStackMonitor)
add_subdirectory(UnitTest_trcStateMachine)
add_subdirectory(UnitTest_trcStaticBuffer)
add_subdirectory(UnitTest_trcString)
add_subdirectory(UnitTest_trcTask)
add_subdirectory(UnitTest_trcTimestamp)
#add_subdirectory(UnitTest_trcEventVerification)
#add_subdirectory(UnitTest_RUNNER)
