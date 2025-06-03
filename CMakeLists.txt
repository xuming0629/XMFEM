cmake_minimum_required(VERSION 3.10.0)
project(XFEM VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()


# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)


# 添加子目录
add_subdirectory(src)
add_subdirectory(examples)


# 启用测试（CTest）
enable_testing()
# 将 tests 目录纳入构建
add_subdirectory(tests)



