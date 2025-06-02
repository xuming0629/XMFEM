/*******************************************************
 * @FileName      : TriangleMesh.h
 * @Time          : 2025-06-02 23:32:59
 * @Author        : XuMing
 * @Email         : 920972751@qq.com
 * @Description   : 三角形网格类定义
 * @Copyright     : XuMing. All Rights Reserved.
 *******************************************************/

#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H


#include "MeshBase.h"
#include <cmath>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "LogManager.h"

// 派生类：三角形网格
class TriangleMesh : public MeshBase
{
public:
    // 构造函数：默认构造一个空网格
    TriangleMesh() = default;

    // 构造函数：通过节点和单元直接构建网格（适用于非结构网格）
    TriangleMesh(const std::vector<Node> &input_nodes, const std::vector<Cell> &input_cells)
    {
        // 检查单元是否全是三角形
        for (const auto &cell : input_cells)
        {
            if (cell.node_ids.size() != 3)
            {
                throw std::invalid_argument("All cells must have exactly 3 node IDs for a triangle mesh.");
            }
        }

        nodes = input_nodes;
        cells = input_cells;

        // 可以在这里按需要排序单元节点
        for (auto &cell : cells)
        {
            sortCellNodes(cell);
        }
    }

    // 生成规则网格节点和单元
    void buildMesh(int nx, int ny, const std::vector<double> &box)
    {
        if (box.size() != 4)
        {
            throw std::invalid_argument("box must have 4 elements: [xmin, xmax, ymin, ymax]");
        }

        double xmin = box[0], xmax = box[1];
        double ymin = box[2], ymax = box[3];

        double dx = (xmax - xmin) / nx;
        double dy = (ymax - ymin) / ny;

        int id = 0;
        nodes.clear();
        cells.clear();

        // 生成节点

        for (int j = 0; j <= ny; ++j)
        {
            for (int i = 0; i <= nx; ++i)
            {
                Node n;
                n.id = id++; // 从0开始
                n.x = xmin + i * dx;
                n.y = ymin + j * dy;
                nodes.push_back(n);
            }
        }

        // 生成单元（三角形）
        for (int j = 0; j < ny; ++j)
        {
            for (int i = 0; i < nx; ++i)
            {
                int n1 = j * (nx + 1) + i ;
                int n2 = n1 + 1;
                int n3 = n1 + (nx + 1);
                int n4 = n3 + 1;

                // 三角形 1: n2 n4 n1
                Cell cell1;
                cell1.node_ids = {n2, n4, n1};
                sortCellNodes(cell1);
                cells.push_back(cell1);

                // 三角形 2: n3 n1 n4
                Cell cell2;
                cell2.node_ids = {n3, n1, n4};
                sortCellNodes(cell2);
                cells.push_back(cell2);
            }
        }
    }

    
    void sortCellNodes(Cell &cell)
    {
        if (cell.node_ids.size() != 3)
        {
            LOG_ERROR("Cell must have exactly 3 nodes");
            throw std::runtime_error("A triangle must have 3 nodes");
        }

        // 取出3个节点
        Node n0 = getNodeById(cell.node_ids[0]);
        Node n1 = getNodeById(cell.node_ids[1]);
        Node n2 = getNodeById(cell.node_ids[2]);

        // 计算叉积判断是否为逆时针
        auto cross = [](const Node &a, const Node &b, const Node &c) -> double
        {
            return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        };

        double cross_product = cross(n0, n1, n2);

        if (cross_product > 0)
        {
            // 已经是逆时针顺序，不用改动
            cell.node_ids = {n0.id, n1.id, n2.id};
        }
        else if (cross_product < 0)
        {
            // 顺时针，交换最后两个点，变成逆时针
            cell.node_ids = {n0.id, n2.id, n1.id};
        }
        else
        {
            // 三点共线（退化三角形），不做排序，或者抛异常
            LOG_ERROR("Degenerate triangle with colinear points: " +
                      std::to_string(n0.id) + ", " +
                      std::to_string(n1.id) + ", " +
                      std::to_string(n2.id));
            std::cerr << "Warning: Degenerate triangle with colinear points: "
                      << n0.id << ", " << n1.id << ", " << n2.id << std::endl;
            // 保持原顺序
        }
    }

    // 获取指定节点
    Node getNodeById(int id) const
    {
        for (const auto &n : nodes)
        {
            if (n.id == id)
                return n;
        }
        throw std::runtime_error("Node ID not found");
    }
    // 根据 ID 查找并返回节点(按值返回

    // 根据 ID 查找并返回节点在 vector 中的索引
    int getNodeIndexById(int id) const
    {
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (nodes[i].id == id)
                return static_cast<int>(i);
        }
        throw std::runtime_error("Node ID not found");
    }
};

#endif // TRIANGLE_MESH_H
