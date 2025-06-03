#include "MeshDS.hpp"
#include <iostream>

using namespace XMFEM;

int main() {
    // 创建一个 2D 网格 MeshDS
    MeshDS mesh(2);

    // 1) 定义 4 个节点（编号 0,1,2,3，仅编号，无坐标信息）
    //    实际上 node 只需要 "占位"，如果你要存坐标，需要单独存一个坐标数组，这里简化为仅存编号。
    //    由于本例不需要直接访问 node_to_node 时索引，我们不专门存 node 实体。

    // 2) 定义 4 条边 (edge->node)
    EntityList edges = {
        {0, 1},  // e0
        {1, 2},  // e1
        {2, 3},  // e2
        {3, 0}   // e3
    };
    mesh.set_entity(1, edges);

    // 3) 定义 1 个单元 (cell->node)
    EntityList cells = {
        {0, 1, 2, 3}  // c0
    };
    mesh.set_entity(3, cells);

    // 4) 定义 face2cell 映射 (edge 也当 face 来处理)
    //    face2cell[i] = { cell0, cell1, local_face_index_in_cell0, local_face_index_in_cell1 }
    //    因为每条边只属于一个单元 cell0，所以 cell1=-1, local_face_index_in_cell1=-1
    mesh.face2cell = {
        {0, -1, 0, -1},  // e0 属于 c0 的第 0 条本地边
        {0, -1, 1, -1},  // e1 属于 c0 的第 1 条本地边
        {0, -1, 2, -1},  // e2 属于 c0 的第 2 条本地边
        {0, -1, 3, -1}   // e3 属于 c0 的第 3 条本地边
    };

    // 5) 定义 localFace（每个 cell 的本地边，对应上面 face2cell 的本地索引顺序）
    //    localFace.size() = 4，表示一个 cell 有 4 条本地边
    mesh.localFace = {
        {0, 1},  // 本地边 0：nodes(0,1)
        {1, 2},  // 本地边 1：nodes(1,2)
        {2, 3},  // 本地边 2：nodes(2,3)
        {3, 0}   // 本地边 3：nodes(3,0)
    };

    // 6) 测试 node->node 邻接
    auto adj = mesh.node_to_node();
    std::cout << "Node-to-node adjacency (COO format):\n";
    for (auto &pr : adj.edges) {
        std::cout << pr.first << " --> " << pr.second << "\n";
    }

    // 7) 测试 cell_to_face
    auto c2f = mesh.cell_to_face();
    std::cout << "\nCell-to-face mapping:\n";
    for (size_t ci = 0; ci < c2f.size(); ++ci) {
        std::cout << "Cell " << ci << ": ";
        for (auto fidx : c2f[ci]) {
            std::cout << fidx << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
