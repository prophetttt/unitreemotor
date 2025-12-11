set(sourcfile "test/coreTest.cpp" 
             "src/unitreeMotorGom.c"
             )

set(motorDataStruct "src/unitreeMotorGom.c")



add_executable(motorDataSturctTest 
src/unitreeMotorGom.c 
test/dataGomTest.cpp)

target_include_directories(motorDataSturctTest PRIVATE "include")


add_test(NAME MotorCoreTest COMMAND run_tests)