add_executable(ex1 ex1.cpp)

# 链接 mesh 和 fem 模块
target_link_libraries(ex1 PRIVATE meshds)

target_include_directories(ex1 PRIVATE
    ${PROJECT_SOURCE_DIR}/src/mesh
)