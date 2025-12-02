cmake_minimum_required(VERSION 3.10)

# 1. 项目名称
project(MotorTunerApp LANGUAGES CXX)

# 2. 设置 C++ 标准 (推荐 C++17 或 C++14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. 查找 wxWidgets 库
# COMPONENTS: 这里列出了你需要的模块
#   - core: 核心 GUI 组件 (wxFrame, wxButton, wxPanel 等)
#   - base: 基础非 GUI 组件 (wxString, wxThread 等)
#   - net:  网络功能 (如果以后需要 socket 通信)
#   - aui:  高级用户界面 (如果你想做可停靠面板，虽然你的截图看起来是静态布局，但加上无妨)
find_package(wxWidgets REQUIRED COMPONENTS core base net aui)

# 4. 包含 wxWidgets 的设置文件
# 这步非常关键，它会设置预处理器定义（如 __WXMSW__）和包含路径
include(${wxWidgets_USE_FILE})

# 5. 定义源文件
set(SOURCE_FILES
    MyApp.cpp
    MainFrame.cpp
    MainFrame.h
    DashboardPanel.cpp
    DashboardPanel.h
    ConfigPanel.cpp
    ConfigPanel.h
)

# 6. 生成可执行文件
# WIN32: 在 Windows 上不仅生成 .exe，还告诉系统这是一个 GUI 程序（不弹出黑色命令行窗口）
# MACOSX_BUNDLE: 在 macOS 上生成 .app 包
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCE_FILES})

# 7. 链接 wxWidgets 库
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})