#include <iostream>
#include "MeshDS.hpp"

int main() {
    MeshDS mesh(3); // 三维网格

    // 设置节点坐标 (示例三个点)
    mesh.set_nodes({
        {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    });

    // 设置单元，假设单元是四面体，4个节点索引
    mesh.set_entity(3, {
        {0, 1, 2, 3}
    });

    // 设置面，假设四个三角面
    mesh.set_entity(2, {
        {0, 1, 2},
        {0, 1, 3},
        {0, 2, 3},
        {1, 2, 3}
    });

    // 查询
    std::cout << "Number of nodes: " << mesh.number_of_nodes() << std::endl;
    std::cout << "Number of cells: " << mesh.number_of_cells() << std::endl;

    // 边界节点
    auto bd_nodes = mesh.boundary_node_index();
    std::cout << "Boundary nodes: ";
    for (int n : bd_nodes) std::cout << n << " ";
    std::cout << std::endl;

    auto bd_flags = mesh.boundary_node_flag();
    std::cout << "Boundary flags: ";
    for (bool f : bd_flags) std::cout << f << " ";
    std::cout << std::endl;

    return 0;
}
