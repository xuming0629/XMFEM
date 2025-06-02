/*******************************************************
 * @FileName      : MeshBase.h
 * @Time          : 2025-06-02 23:32:59
 * @Author        : XuMing
 * @Email         : 920972751@qq.com
 * @Description   : 网格基础类定义
 * @Copyright     : XuMing. All Rights Reserved.
 *******************************************************/


#ifndef MESH_BASE_H
#define MESH_BASE_H


#include <vector>
#include <string>
#include <iostream>


// 网格节点
struct Node
{
    int id;                    // 节点的唯一标识符  
    double x, y;                // 节点的坐标
};

// 网格单元
struct Cell
{
    int id;                    // 单元的唯一标识符
    std::vector<int> node_ids; // 三角形单元一般存在 3 个节点
};


// 网格边
struct Edge
{
    int id;                 // 边的唯一标识符
    int node1_id, node2_id; // 边由两个节点定义
};


class MeshBase
{
public:
    std::vector<Node> nodes;
    std::vector<Cell> cells;
    std::vector<Edge> edges; 


    // 节点的个数
    int number_of_nodes() const
    {
        return static_cast<int>(nodes.size());
    }

    // 单元的个数
    int number_of_cells() const
    {
        return static_cast<int>(cells.size());
    }

    // 边的个数
   int number_of_edges() const
    {
        return static_cast<int>(edges.size());
    }


    // 拓扑维数
    int topological_dimension() const
    {
        return 2; // 三角形网格的拓扑维数为 2
    }   

    // 几何维数

    int geometrical_dimension() const
    {
        return 2; // 三角形网格的几何维数为 2
    }
 
};

#endif // MESH_BASE_H