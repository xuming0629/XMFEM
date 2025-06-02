#include "TriangleMesh.h"
#include <iostream>
#include "LogManager.h"
// #include "LagrangeFEMSpace.h"

int main() {

    LOG_DEBUG("Start to build mesh");
    LOG_ERROR("This is an error message");
    TriangleMesh mesh;
    mesh.buildMesh(2, 2, {0, 1, 0, 1});  // 2x2 网格，单元大小 1.0

    LOG_DEBUG("\n=========================== Nodes Infomation ===========================" );
    // 打印节点
    for (const auto& n : mesh.nodes) {
        std::cout << "Node " << n.id << ": (" << n.x << ", " << n.y << ")\n";
    }
    LOG_DEBUG("\n=========================== NOdes End ===========================" );

    // 打印单元
    LOG_DEBUG("\n=========================== Cells Infomation ===========================" );
    int cell_id = 1;
    for (const auto& c : mesh.cells) {
        std::cout << "Cell " << cell_id++ << ": ";
        for (auto nid : c.node_ids)
            std::cout << nid << " ";
        std::cout << "\n";
    }
    LOG_DEBUG("\n=========================== Cells End ===========================" );

    // LagrangeFEMSpace fem_space(&mesh);
    // int total_dofs = fem_space.getNumDofs();
    // std::cout << "Total DOFs: " << total_dofs << std::endl;

    // double v = fem_space.basisValue(1, 0.3, 0.2);
    // std::cout << "phi_1(0.3, 0.2) = " << v << std::endl;



    return 0;
}
