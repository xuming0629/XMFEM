#ifndef MESH_DS_HPP_
#define MESH_DS_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <set>

class MeshDS {
public:
    using Node = std::vector<std::vector<double>>;      // 节点坐标列表，二维坐标或三维坐标
    using Entity = std::vector<std::vector<int>>;       // 单元、边、面等索引列表

public:
    explicit MeshDS(int TD_) : TD(TD_) {}

    int top_dimension() const { return TD; }

    // 获取实体（节点除外）
    const Entity& entity(int etype) const {
        auto it = entity_storage.find(etype);
        if (it != entity_storage.end()) {
            return it->second;
        } else {
            throw std::out_of_range("Invalid entity type.");
        }
    }

    // 添加实体
    void set_entity(int etype, const Entity& ents) {
        entity_storage[etype] = ents;
    }

    // 设置节点坐标
    void set_nodes(const Node& nodes_) {
        nodes = nodes_;
    }

    // 获取节点坐标
    const Node& get_nodes() const {
        return nodes;
    }

    // 获取实体个数
    size_t number_of_nodes() const {
        return nodes.size();
    }

    size_t number_of_cells() const {
        return entity_storage.count(TD) ? entity_storage.at(TD).size() : 0;
    }

    size_t number_of_edges() const {
        return entity_storage.count(1) ? entity_storage.at(1).size() : 0;
    }

    size_t number_of_faces() const {
        return entity_storage.count(TD - 1) ? entity_storage.at(TD - 1).size() : 0;
    }

    size_t number_of_vertices_of_cell() const {
        if(entity_storage.count(TD) == 0) return 0;
        return entity_storage.at(TD)[0].size();
    }

    // 计算边界节点索引（示例）
    std::vector<int> boundary_node_index() const {
        // 简单示例：找所有边界面上的节点
        // 这里只做示例，实际需结合面与单元关系判断边界

        if(entity_storage.count(TD-1) == 0) return {};

        const Entity& faces = entity_storage.at(TD-1);
        std::set<int> bd_nodes;
        // 这里假设所有面都是边界面（示例），实际要判断面是否共享两个单元
        for (const auto& face : faces) {
            for (int nid : face) {
                bd_nodes.insert(nid);
            }
        }

        return std::vector<int>(bd_nodes.begin(), bd_nodes.end());
    }

    // 计算边界节点标志向量
    std::vector<bool> boundary_node_flag() const {
        std::vector<bool> flags(number_of_nodes(), false);
        auto bd_nodes = boundary_node_index();
        for (int nid : bd_nodes) {
            if(nid >= 0 && static_cast<size_t>(nid) < flags.size())
                flags[nid] = true;
        }
        return flags;
    }

    // 其它类似功能可以照搬或自己扩展

private:
    int TD;  // 拓扑维度

    Node nodes;  // 节点坐标列表

    // key: 实体维度，如 0=node(不存?),1=edge,2=face,3=cell
    std::map<int, Entity> entity_storage;
};

#endif // MESH_DS_HPP_
