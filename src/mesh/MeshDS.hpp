#ifndef MESHDS_HPP
#define MESHDS_HPP

#include <vector>
#include <unordered_map>
#include <utility>

namespace XMFEM {

// 索引类型别名
using Index = int;

// 用于表示各种实体（node/edge/face/cell 等）的列表。
// 外层 vector 按实体编号（从 0 开始）索引，内层 vector 存储该实体所包含的节点编号。
using EntityList = std::vector<std::vector<Index>>;

/**
 * @brief 稀疏邻接矩阵（COO 格式），仅用于 node-to-node 等稀疏关系
 * 
 * .edges 存储所有 (i,j) 对，表示行 i 列 j 有一条边
 * .nrows = 行数，.ncols = 列数
 */
struct SparseAdjacency {
    std::vector<std::pair<Index, Index>> edges;
    int nrows = 0;
    int ncols = 0;
};

/**
 * @brief MeshDS: 网格数据结构存储与基本拓扑关系
 * 
 * 仅使用标准 C++ STL，不依赖任何外部 tensor 库。
 * 支持：
 *   - node / edge / face / cell 实体的存储
 *   - 基本的拓扑查询：cell->node、cell->face、node->node（邻接）
 */
class MeshDS {
public:
    /**
     * @param topDim 拓扑维度（在 2D 情况一般设为 2，3D 情况设为 3）
     */
    MeshDS(int topDim = 2);

    /// 设置某一维度实体（dim: 0=node,1=edge,2=face,3=cell）
    void set_entity(int dim, const EntityList& entity);

    /// 获取某一维度实体（常量引用）
    const EntityList& entity(int dim) const;

    /// 返回指定维度实体数量（不存在时返回 0）
    int count(int dim) const;

    /// 各维度快捷接口
    int number_of_nodes() const;   // dim=0
    int number_of_edges() const;   // dim=1
    int number_of_faces() const;   // dim=2
    int number_of_cells() const;   // dim=3

    /// cell->node: 直接返回 3 维度（dim=3）对应的列表
    const EntityList& cell_to_node() const;

    /**
     * @brief 计算 cell->face 的映射
     * 
     * 返回一个形状为 (NC × NFC) 的 EntityList，
     *   NC = number_of_cells(),
     *   NFC = localFace 每个 cell 的局部 face 数目。
     * localFace[i] = {n0, n1, …} 表示第 i 个“本地面”（在 cell 中）的节点编号顺序，
     * face2cell 的存储格式：每行 4 个 int，分别是 { cell0, cell1, local_face_index_in_cell0, local_face_index_in_cell1 }。
     */
    EntityList cell_to_face() const;

    /**
     * @brief 计算 node->node 的邻接稀疏矩阵（COO 格式）
     * 
     * 遍历每条 edge，将 (n0,n1) 和 (n1,n0) 都加入 edges 数组。
     */
    SparseAdjacency node_to_node() const;

    /// 面到单元（face2cell）和本地面编号(localFace)都要在外部直接赋值，故公开为 public
    EntityList face2cell;   // 每行 {cell0, cell1, local_face0, local_face1}
    EntityList localFace;   // 每个 cell 的“本地面”节点组合

private:
    int TD;  // 拓扑维度

    /// 核心存储：dim -> “实体列表”。dim=0,1,2,3 分别对应 node/edge/face/cell
    std::unordered_map<int, EntityList> entity_storage;
};

} // namespace XMFEM

#endif // MESHDS_HPP
