#ifndef LAGRANGE_FEM_SPACE_H
#define LAGRANGE_FEM_SPACE_H

#include "TriangleMesh.h"
#include <vector>
#include <utility>
#include <cmath>
#include <stdexcept>
#include <Eigen/Dense>  // 用 Eigen 来做矩阵操作，方便高效

using namespace Eigen;

// 一阶（线性）Lagrange FEM 空间 + Poisson 求解器集成
class LagrangeFEMSpace {
public:
    TriangleMesh* mesh;                // 三角形网格
    int dof_per_node = 1;              // 每个节点一个自由度

    std::vector<std::vector<int>> cell_dof_ids;  // 单元到自由度编号映射
    std::vector<int> global_dof_ids;             // 节点到自由度编号映射

    // 构造：传入已 build 完的 TriangleMesh
    LagrangeFEMSpace(TriangleMesh* m) : mesh(m) {
        assignDof();
    }

    // 分配全局自由度编号
    void assignDof() {
        int n = mesh->nodes.size();
        global_dof_ids.resize(n);
        for (int i = 0; i < n; ++i) {
            global_dof_ids[i] = i;
        }
        cell_dof_ids.clear();
        for (auto& cell : mesh->cells) {
            std::vector<int> dofs;
            for (int nid : cell.node_ids) {
                int idx = mesh->getNodeIndexById(nid);
                dofs.push_back(global_dof_ids[idx]);
            }
            cell_dof_ids.push_back(dofs);
        }
    }

    // 总自由度数
    int getNumDofs() const {
        return mesh->nodes.size() * dof_per_node;
    }

    // 参考单元基函数值
    double basisValue(int i, double xi, double eta) const {
        switch(i) {
            case 0: return 1 - xi - eta;
            case 1: return xi;
            case 2: return eta;
            default: throw std::invalid_argument("basisValue index out of range");
        }
    }
    // 参考单元基函数对 xi、eta 导数
    double basisGradXi(int i) const {
        switch(i) { case 0: return -1; case 1: return 1; case 2: return 0; }
        throw std::invalid_argument("basisGradXi index out of range");
    }
    double basisGradEta(int i) const {
        switch(i) { case 0: return -1; case 1: return 0; case 2: return 1; }
        throw std::invalid_argument("basisGradEta index out of range");
    }

    // 单元刚度矩阵 (3x3)
    Matrix3d computeElementStiffnessMatrix(int c) const {
        const auto& cell = mesh->cells[c];
        // 获取节点坐标
        Node n0 = mesh->getNodeById(cell.node_ids[0]);
        Node n1 = mesh->getNodeById(cell.node_ids[1]);
        Node n2 = mesh->getNodeById(cell.node_ids[2]);
        Vector2d p0(n0.x, n0.y), p1(n1.x, n1.y), p2(n2.x, n2.y);
        // Jacobian
        Matrix2d J; J << p1 - p0, p2 - p0;
        double detJ = J.determinant();
        double area = 0.5 * std::abs(detJ);
        // 参考基函数梯度
        Matrix<double,2,3> grad_ref;
        for(int i=0;i<3;++i){ grad_ref(0,i)=basisGradXi(i); grad_ref(1,i)=basisGradEta(i); }
        // 物理梯度
        Matrix2d invJ = J.inverse();
        Matrix<double,2,3> grad = invJ * grad_ref;
        // 组装 Ke
        Matrix3d Ke = Matrix3d::Zero();
        for(int i=0;i<3;++i)
            for(int j=0;j<3;++j)
                Ke(i,j) = area * grad.col(i).dot(grad.col(j));
        return Ke;
    }

    // 组装全局刚度矩阵 K
    void assembleStiffness(MatrixXd& K) const {
        int nd = getNumDofs();
        K = MatrixXd::Zero(nd, nd);
        for(int c=0;c<mesh->cells.size();++c) {
            Matrix3d Ke = computeElementStiffnessMatrix(c);
            auto& dofs = cell_dof_ids[c];
            for(int i=0;i<3;++i)
                for(int j=0;j<3;++j)
                    K(dofs[i], dofs[j]) += Ke(i,j);
        }
    }

    // 组装载荷向量 F (常数源项 f=1)
    void assembleLoad(VectorXd& F) const {
        int nd = getNumDofs();
        F = VectorXd::Zero(nd);
        for(int c=0;c<mesh->cells.size();++c) {
            const auto& cell = mesh->cells[c];
            Node n0 = mesh->getNodeById(cell.node_ids[0]);
            Node n1 = mesh->getNodeById(cell.node_ids[1]);
            Node n2 = mesh->getNodeById(cell.node_ids[2]);
            Vector2d p0(n0.x,n0.y), p1(n1.x,n1.y), p2(n2.x,n2.y);
            double area = 0.5 * std::abs((p1 - p0).x()*(p2 - p0).y() - (p2 - p0).x()*(p1 - p0).y());
            Vector3d Fe = Vector3d::Ones() * (area/3.0);
            auto& dofs = cell_dof_ids[c];
            for(int i=0;i<3;++i) F(dofs[i]) += Fe(i);
        }
    }

    // 强制 Dirichlet 边界
    void applyDirichletBC(MatrixXd& K, VectorXd& F, const std::vector<int>& bnodes, double val=0) const {
        for(int nid: bnodes) {
            int d = global_dof_ids[mesh->getNodeIndexById(nid)];
            K.row(d).setZero(); K.col(d).setZero(); K(d,d)=1;
            F(d) = val;
        }
    }

    // Neumann 边界条件（边源项）
    void applyNeumannBC(VectorXd& F, const std::vector<std::pair<int,int>>& edges, double g) const {
        for(auto& e: edges) {
            Node n1 = mesh->getNodeById(e.first);
            Node n2 = mesh->getNodeById(e.second);
            Vector2d p1(n1.x,n1.y), p2(n2.x,n2.y);
            double L = (p2-p1).norm();
            double c = g * L/2.0;
            int d1 = global_dof_ids[mesh->getNodeIndexById(e.first)];
            int d2 = global_dof_ids[mesh->getNodeIndexById(e.second)];
            F(d1)+=c; F(d2)+=c;
        }
    }

    // Robin 边界条件 α u + β ∂u/∂n = g, β=1
    void applyRobinBC(MatrixXd& K, VectorXd& F,
                      const std::vector<std::pair<int,int>>& edges,
                      double alpha, double g) const {
        for(auto& e: edges) {
            Node n1 = mesh->getNodeById(e.first);
            Node n2 = mesh->getNodeById(e.second);
            Vector2d p1(n1.x,n1.y), p2(n2.x,n2.y);
            double L = (p2-p1).norm();
            double coeff = alpha*L/6.0;
            int d1=global_dof_ids[mesh->getNodeIndexById(e.first)];
            int d2=global_dof_ids[mesh->getNodeIndexById(e.second)];
            K(d1,d1)+=2*coeff; K(d1,d2)+= coeff;
            K(d2,d1)+= coeff;   K(d2,d2)+=2*coeff;
            double lf = g * L/2.0;
            F(d1)+=lf; F(d2)+=lf;
        }
    }

    // Poisson 求解：f=1, Dirichlet/Neumann/Robin 组合
    VectorXd solvePoisson(const std::vector<int>& dirichlet_nodes,
                          const std::vector<std::pair<int,int>>& neu_edges = {},
                          const std::vector<std::pair<int,int>>& rob_edges = {},
                          double alpha=0, double g_neu=0, double g_rob=0) const {
        MatrixXd K; assembleStiffness(K);
        VectorXd F; assembleLoad(F);
        if(!neu_edges.empty()) applyNeumannBC(F, neu_edges, g_neu);
        if(!rob_edges.empty()) applyRobinBC(K, F, rob_edges, alpha, g_rob);
        applyDirichletBC(K, F, dirichlet_nodes);
        // 求解
        Eigen::SimplicialLLT<SpMat> solver;
        solver.compute(K.sparseView());
        return solver.solve(F);
    }
};

#endif // LAGRANGE_FEM_SPACE_H
