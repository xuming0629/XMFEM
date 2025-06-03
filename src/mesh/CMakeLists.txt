# 生成静态库
add_library(meshds MeshDS.cpp)

target_include_directories(meshds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})