#ifndef MESH_BASE_H
#define MESH_BASE_H

#include <vector>
#include <string>
#include <iostream>


struct Node {
    int id;
    double x, y;
};  

struct Cell {
    int id;
    std::vector<int> node_ids; // 三角形单元一般存在 3 个节点
};

class MeshBase {
    public:
    std::vector<Node> nodes;
    std::vector<Cell> cells;

    // 获取指定节点
    Node getNodeById(int id) const {
        for (const auto& n : nodes) {
            if (n.id == id) return n;
        }
        throw std::runtime_error("Node ID not found");
    }
    // 根据 ID 查找并返回节点(按值返回

    // 根据 ID 查找并返回节点在 vector 中的索引
    int getNodeIndexById(int id) const {
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i].id == id) return static_cast<int>(i);
        }
        throw std::runtime_error("Node ID not found");
}

    



};
    

#endif // MESH_BASE_H