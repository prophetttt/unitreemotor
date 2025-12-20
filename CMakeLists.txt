cmake_minimum_required(VERSION 3.18)


project(MotorTunerApp LANGUAGES CXX C VERSION 0.1)

set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0076 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)

message(STATUS "CMake version ${CMAKE_VERSION}")

if(MSVC)
  if(MSVC_VERSION LESS 1920)
    message(FATAL_ERROR "Need Visual Studio 2019 or newer to build XGBoost")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.1")
    message(FATAL_ERROR "Need GCC 8.1 or newer to build XGBoost")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.0")
    message(FATAL_ERROR "Need Xcode 11.0 (AppleClang 11.0) or newer to build XGBoost")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    message(FATAL_ERROR "Need Clang 9.0 or newer to build XGBoost")
  endif()
endif()

set(CMAKE_C_STANDARD 17)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(gui)
add_subdirectory(core)