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

    const std::vector<Index>& get_entity(int etype, int index) const;
    const std::vector<Index>& get_entity(const std::string& etype, int index) const;


    void set_entity(int etype, const Tensor& entity);
    

     void clear();

    int top_dimension() const;
    size_t count(int etype) const;

    
   std::vector<std::pair<Index, Index>> edge_to_node() const;
    std::vector<std::vector<Index>> face_to_node() const;
    std::vector<std::vector<Index>> cell_to_edge() const;

    std::vector<Index> node_to_cell(Index node) const;

    void print_entity(int etype) const;
    void print_summary() const;

    int number_of_nodes() const { return count(0); }
    int number_of_edges() const { return count(1); }
    int number_of_faces() const { return count(2); }
    int number_of_cells() const { return count(3); }


    int number_of(const std::string& etype) const {
        return count(estr2dim(etype));
    }

    int estr2dim(const std::string& etype) const;


    // face_to_node, edge_to_node, face_to_cell, cell_to_face, cell_to_edge, cell_to_node
    // node_to_cell, edge_to_cell, face_to_cell
    // cell_to_edge, cell_to_face, cell_to_node
    //

   

    int estr2dim(const std::string& etype) {
    if (etype == "node") return 0;
    if (etype == "edge") return 1;
    if (etype == "face") return 2;
    if (etype == "cell") return 3;
    throw std::invalid_argument("Unknown entity type: " + etype);
}


private:
    int TD;
    std::map<int, Tensor> entity_storage;
};

} // namespace XMFEM

#endif
