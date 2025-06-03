#include "MeshDS.hpp"
#include <cassert>

namespace XMFEM {

MeshDS::MeshDS(int topDim)
    : TD(topDim)
{
    // 构造时不自动分配任何实体，需要调用 set_entity
}

//------------------------------------------------------------------------------------------------
// 设置某一维度实体：dim 取值 0(node)、1(edge)、2(face)、3(cell)
void MeshDS::set_entity(int dim, const EntityList& entity) {
    entity_storage[dim] = entity;
}

//------------------------------------------------------------------------------------------------
// 获取某一维度实体（如果不存在，断言失败）
const EntityList& MeshDS::entity(int dim) const {
    auto it = entity_storage.find(dim);
    assert(it != entity_storage.end() && "entity(dim) not found");
    return it->second;
}

//------------------------------------------------------------------------------------------------
// 返回指定维度的实体数量（不存在则返回 0）
int MeshDS::count(int dim) const {
    auto it = entity_storage.find(dim);
    if (it == entity_storage.end()) return 0;
    return static_cast<int>(it->second.size());
}

int MeshDS::number_of_nodes() const { return count(0); }
int MeshDS::number_of_edges() const { return count(1); }
int MeshDS::number_of_faces() const { return count(2); }
int MeshDS::number_of_cells() const { return count(3); }

//------------------------------------------------------------------------------------------------
// 直接返回 cell->node
const EntityList& MeshDS::cell_to_node() const {
    return entity(3);
}

//------------------------------------------------------------------------------------------------
// 计算 cell->face 映射：
//   输出：一个 NC × NFC 的 EntityList，NC=number_of_cells(), NFC=localFace[i].size()
//   face2cell 每行存 {c0, c1, loc0, loc1}。
EntityList MeshDS::cell_to_face() const {
    int NC = number_of_cells();
    int NF = static_cast<int>(face2cell.size());
    int NFC = 0;
    if (!localFace.empty()) {
        // 假设每个 cell 都有相同数量的本地面
        NFC = static_cast<int>(localFace.size());
    }

    // 初始化结果表，初始值都为 -1
    EntityList result(NC, std::vector<Index>(NFC, -1));

    for (int i = 0; i < NF; ++i) {
        // face2cell[i] = { cell0, cell1, local_face_index_in_cell0, local_face_index_in_cell1 }
        const auto& fc = face2cell[i];
        assert(fc.size() == 4);
        Index c0 = fc[0];
        Index c1 = fc[1];
        Index loc0 = fc[2];
        Index loc1 = fc[3];

        // 把全局 face i 填到对应的 cell->face 位置
        if (c0 >= 0 && loc0 >= 0 && loc0 < NFC) {
            result[c0][loc0] = i;
        }
        if (c1 >= 0 && loc1 >= 0 && loc1 < NFC) {
            result[c1][loc1] = i;
        }
    }
    return result;
}

//------------------------------------------------------------------------------------------------
// 计算 node->node 邻接，稀疏表示 (COO 格式)
SparseAdjacency MeshDS::node_to_node() const {
    SparseAdjacency adj;
    int NN = number_of_nodes();
    adj.nrows = NN;
    adj.ncols = NN;

    // 从 edge 实体获取所有边对
    const auto& edge_list = entity(1);
    for (const auto& e : edge_list) {
        assert(e.size() == 2);
        Index n0 = e[0];
        Index n1 = e[1];
        // 加双向
        adj.edges.emplace_back(n0, n1);
        adj.edges.emplace_back(n1, n0);
    }
    return adj;
}

} // namespace XMFEM
