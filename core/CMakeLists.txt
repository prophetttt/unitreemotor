set(sourcfile "test/coreTest.cpp")
add_executable(motorBack ${sourcfile})

add_test(NAME MotorCoreTest COMMAND run_tests)