#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "MeshBase.h"
#include <cmath>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "Eigen/Dense"
#include "LogManager.h"

// 派生类：三角形网格
class TriangleMesh : public MeshBase
{
public:
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

        int id = 1;
        nodes.clear();
        cells.clear();

        // 生成节点
        for (int j = 0; j <= ny; ++j)
        {
            for (int i = 0; i <= nx; ++i)
            {
                Node n;
                n.id = id++;
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
                int n1 = j * (nx + 1) + i + 1;
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

    // 给定一个 Cell，按逆时针顺序排序，并确保第一个点是直角点
    void sortCellNodes(Cell &cell)
    {
        if (cell.node_ids.size() != 3)
        {
            LOG_ERROR("Cell must have exactly 3 nodes");
            throw std::runtime_error("A triangle must have 3 nodes");
        }

        // 获取节点对象
        Node n0 = getNodeById(cell.node_ids[0]);
        Node n1 = getNodeById(cell.node_ids[1]);
        Node n2 = getNodeById(cell.node_ids[2]);

        std::vector<std::pair<Node, int>> nodes_with_ids = {
            {n0, cell.node_ids[0]},
            {n1, cell.node_ids[1]},
            {n2, cell.node_ids[2]}};

        // 找到直角点（两条边垂直）
        int right_angle_index = -1;
        for (int i = 0; i < 3; ++i)
        {
            Node a = nodes_with_ids[i].first;
            Node b = nodes_with_ids[(i + 1) % 3].first;
            Node c = nodes_with_ids[(i + 2) % 3].first;

            double dx1 = b.x - a.x;
            double dy1 = b.y - a.y;
            double dx2 = c.x - a.x;
            double dy2 = c.y - a.y;

            double dot_product = dx1 * dx2 + dy1 * dy2;

            if (std::abs(dot_product) < 1e-8)
            {
                right_angle_index = i;
                break;
            }
        }

        if (right_angle_index == -1)
        {
            // 没找到直角点，就默认不排序
            LOG_ERROR("No right angle found in cell" + std::to_string(cell.node_ids[0]) + ", " +
                      std::to_string(cell.node_ids[1]) + ", " + std::to_string(cell.node_ids[2]));
            std::cerr << "Warning: No right angle found in cell " << cell.node_ids[0] << ", " << cell.node_ids[1] << ", " << cell.node_ids[2] << std::endl;
            return;
        }

        // 以直角点开头排好顺序，逆时针
        std::vector<int> ordered_ids = {
            nodes_with_ids[right_angle_index].second,
            nodes_with_ids[(right_angle_index + 1) % 3].second,
            nodes_with_ids[(right_angle_index + 2) % 3].second};

        // 判断是否逆时针（通过叉积）
        Node A = getNodeById(ordered_ids[0]);
        Node B = getNodeById(ordered_ids[1]);
        Node C = getNodeById(ordered_ids[2]);

        double cross_product = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
        if (cross_product < 0)
        {
            std::swap(ordered_ids[1], ordered_ids[2]);
        }

        cell.node_ids = ordered_ids;
    }

    int getNodeIndexById(int id) const
    {
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            if (nodes[i].id == id)
                return i;
        }
        throw std::runtime_error("Node ID not found");
    }


};

#endif // TRIANGLE_MESH_H
