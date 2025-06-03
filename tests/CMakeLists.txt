# XMFEM/tests/CMakeLists.txt

# 1) 先定义可执行测试目标
add_executable(test_meshds
    test_meshds.cpp
)

# 2) 指定 test_meshds 可执行文件需要包含哪些头文件目录：
#    - ${PROJECT_SOURCE_DIR}/src/mesh   → 可以找到 MeshDS.hpp
#    - ${PROJECT_SOURCE_DIR}/include    → 可以找到 XMFEMConfig.hpp（如果测试里用到）
target_include_directories(test_meshds PRIVATE
    ${PROJECT_SOURCE_DIR}/src/mesh
    ${PROJECT_SOURCE_DIR}/include
)

# 3) 链接 meshds 库
#    “meshds” 必须与 src/mesh/CMakeLists.txt 中 add_library(meshds ...) 的 target 名称一致
target_link_libraries(test_meshds PRIVATE meshds)

# 4) 注册测试项到 CTest
add_test(
    NAME meshds_unit
    COMMAND test_meshds
)
