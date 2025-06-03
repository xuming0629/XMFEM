#include "MeshDS.hpp"
#include <iostream>

using namespace XMFEM;


int main() {
    MeshDS mesh(2); // 3D 网格

    // 添加 edge 实体（2个点组成）
    XMFEM::Tensor edges = { {0, 1}, {1, 2}, {2, 3} };
    mesh.set_entity(1, edges);

    // 打印 edges
    mesh.print_entity(1);

    // 统计数量
    std::cout << "Number of edges: " << mesh.count(1) << std::endl;

    // 拓扑关系 edge_to_node
    auto e2n = mesh.edge_to_node();
    for (auto [a, b] : e2n)
        std::cout << a << " - " << b << std::endl;


std::cout << "Done!" << std::endl;
std::cout 
    << "Cells: " << mesh.number_of_cells() << "\n"
    << "Edges: " << mesh.number_of_edges() << "\n"
    << "Nodes: " << mesh.number_of_nodes() << "\n"
    << "Faces: " << mesh.number_of_faces() << std::endl;

return 0;

}

