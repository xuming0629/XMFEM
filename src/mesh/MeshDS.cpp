#include "MeshDS.hpp"

namespace XMFEM {

// 构造函数，设置拓扑维度
MeshDS::MeshDS(int topDim) : TD(topDim) {}

// 获取指定实体类型的 Tensor
const Tensor& MeshDS::get_entity(int etype) const {
    return entity_storage.at(etype);
}

// 设置某个实体类型对应的 Tensor
void MeshDS::set_entity(int etype, const Tensor& entity) {
    entity_storage[etype] = entity;
}

// 清空所有实体
void MeshDS::clear() {
    entity_storage.clear();
}

// 获取拓扑维度
int MeshDS::top_dimension() const {
    return TD;
}

// 获取指定实体类型数量
size_t MeshDS::count(int etype) const {
    auto it = entity_storage.find(etype);
    if (it != entity_storage.end())
        return it->second.size();
    else
        return 0;
}

// 获取所有边到节点的映射对
std::vector<std::pair<Index, Index>> MeshDS::edge_to_node() const {
    std::vector<std::pair<Index, Index>> edges;
    auto it = entity_storage.find(1);  // 假设 1 表示边类型
    if (it != entity_storage.end()) {
        for (const auto& edge : it->second) {
            if (edge.size() >= 2)
                edges.emplace_back(edge[0], edge[1]);
        }
    }
    return edges;
}

// 打印某个实体类型下的所有实体内容
void MeshDS::print_entity(int etype) const {
    auto it = entity_storage.find(etype);
    if (it != entity_storage.end()) {
        for (const auto& elem : it->second) {
            for (Index idx : elem)
                std::cout << idx << " ";
            std::cout << std::endl;
        }
    } else {
        std::cout << "Entity type " << etype << " not found." << std::endl;
    }
}

} // namespace XMFEM
