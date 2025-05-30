cmake_minimum_required(VERSION 3.10.0)
project(XMFEM VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加 include 目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 将 src 中所有 cpp 文件加入变量 SRC_FILES
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
# find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
# list(APPEND CMAKE_PREFIX_PATH "/home/xuming/software/anaconda3/lib/python3.12/site-packages/pybind11/share/cmake/pybind11")
# find_package(pybind11 REQUIRED)


# 生成可执行文件
add_executable(main ${SRC_FILES})

# 链接 Eigen（如果用到）
# find_package(Eigen3 REQUIRED)
# target_link_libraries(main PRIVATE pybind11::embed Python3::Python)

# 启用测试
include(CTest)
enable_testing()


