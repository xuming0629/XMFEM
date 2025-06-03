#ifndef MESHDS_HPP
#define MESHDS_HPP

#include <iostream>
#include <vector>
#include <map>

namespace XMFEM {

using Index = std::size_t;
using Tensor = std::vector<std::vector<Index>>;

class MeshDS {
public:
    MeshDS(int topDim);

    
    const Tensor& get_entity(int etype) const;

    void set_entity(int etype, const Tensor& entity);

    void clear();

    int top_dimension() const;

    size_t count(int etype) const;

    // face_to_node, edge_to_node, face_to_cell, cell_to_face, cell_to_edge, cell_to_node
    // node_to_cell, edge_to_cell, face_to_cell
    // cell_to_edge, cell_to_face, cell_to_node
    //

    std::vector<std::pair<Index, Index>> edge_to_node() const;
    void print_entity(int etype) const;


    int number_of_nodes() const { return count(0); }
    int number_of_edges() const { return count(1); }
    int number_of_faces() const { return count(2); }
    int number_of_cells() const { return count(3); }



private:
    int TD;
    std::map<int, Tensor> entity_storage;
};

} // namespace XMFEM

#endif
