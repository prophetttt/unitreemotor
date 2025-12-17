

set(motorDataStruct "src/unitreeMotorGom.c")
set(LIB_SOURCES
    src/coreThread.cpp
    src/serial_initalize.cpp
    src/unitreeMotorGom.c
)
add_library(motor_lib STATIC ${LIB_SOURCES})
target_include_directories(motor_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(motor_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(crcTest test/crcTest.cpp)
target_link_libraries(crcTest PRIVATE motor_lib)

add_executable(motorDataSturctTest
test/dataGomTest.cpp)
target_link_libraries(motorDataSturctTest PRIVATE motor_lib)

add_executable(coreThread test/coreTest.cpp)
target_link_libraries(coreThread PRIVATE motor_lib)

add_test(NAME MotorCoreTest COMMAND run_tests)