cmake_minimum_required(VERSION 3.10)

# 1. 项目名称
project(MotorTunerApp LANGUAGES CXX)

# 2. 设置 C++ 标准 (推荐 C++17 或 C++14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(gui)