

set(motorDataStruct "src/unitreeMotorGom.c")

add_executable(crcTest ${motorDataStruct} test/crcTest.cpp)

# add_executable(motorDataSturctTest 
# src/unitreeMotorGom.c 
# test/dataGomTest.cpp)

# target_include_directories(motorDataSturctTest PRIVATE "include")
target_include_directories(crcTest PRIVATE "include")

add_executable(coreThread src/coreThread.cpp ${motorDataStruct} test/coreTest.cpp)
target_include_directories(coreThread PRIVATE "include")


add_test(NAME MotorCoreTest COMMAND run_tests)