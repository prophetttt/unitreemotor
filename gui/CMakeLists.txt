

find_package(wxWidgets REQUIRED COMPONENTS core base net aui)

# 让编译器能够找到本目录下的头文件（gui/include）
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include(${wxWidgets_USE_FILE})

set(SOURCE_FILES
    src/MyApp.cpp
    src/MainFrame.cpp
    src/ConfigPanel.cpp
)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} motor_lib)

target_compile_options(${PROJECT_NAME} PRIVATE
    -finput-charset=UTF-8
    -fexec-charset=UTF-8
)