#include "MeshDS.hpp"

namespace XMFEM {

MeshDS::MeshDS(int topDim) : TD(topDim) {}

const Tensor& MeshDS::get_entity(int etype) const {
    auto it = entity_storage.find(etype);
    if (it == entity_storage.end())
        throw std::out_of_range("Entity type not found.");
    return it->second;
}

const std::vector<Index>& MeshDS::get_entity(int etype, int index) const {
    auto it = entity_storage.find(etype);
    if (it == entity_storage.end())
        throw std::out_of_range("Entity type not found.");
    if (index < 0 || index >= static_cast<int>(it->second.size()))
        throw std::out_of_range("Entity index out of range.");
    return it->second.at(index);
}

const std::vector<Index>& MeshDS::get_entity(const std::string& etype, int index) const {
    return get_entity(estr2dim(etype), index);
}

void MeshDS::set_entity(int etype, const Tensor& entity) {
    entity_storage[etype] = entity;
}

void MeshDS::clear() {
    entity_storage.clear();
}

int MeshDS::top_dimension() const {
    return TD;
}

size_t MeshDS::count(int etype) const {
    auto it = entity_storage.find(etype);
    if (it == entity_storage.end()) return 0;
    return it->second.size();
}

std::vector<std::pair<Index, Index>> MeshDS::edge_to_node() const {
    std::vector<std::pair<Index, Index>> result;
    auto it = entity_storage.find(1);
    if (it == entity_storage.end()) return result;

    for (const auto& edge : it->second) {
        if (edge.size() != 2) continue;
        result.emplace_back(edge[0], edge[1]);
    }
    return result;
}

std::vector<std::vector<Index>> MeshDS::face_to_node() const {
    auto it = entity_storage.find(2);
    if (it == entity_storage.end()) return {};
    return it->second;
}

std::vector<std::vector<Index>> MeshDS::cell_to_edge() const {
    auto it = entity_storage.find(3);
    if (it == entity_storage.end()) return {};
    return it->second;
}

std::vector<Index> MeshDS::node_to_cell(Index node) const {
    std::vector<Index> result;
    // auto it = entity_storage.find(3);
    // if (it == entity_storage.end()) return result;

    // const auto& cells = it->second;
    // for (Index i = 0; i < cells.size(); ++i) {
    //     if (std::find(cells[i].begin(), cells[i].end(), node) != cells[i].end()) {
    //         result.push_back(i);
    //     }
    // }
    return result;
}

void MeshDS::print_entity(int etype) const {
    auto it = entity_storage.find(etype);
    if (it == entity_storage.end()) {
        std::cout << "Entity type " << etype << " not found.\n";
        return;
    }
    std::cout << "Entities of type " << etype << ":\n";
    for (const auto& e : it->second) {
        for (auto v : e)
            std::cout << v << " ";
        std::cout << std::endl;
    }
}

void MeshDS::print_summary() const {
    std::cout << "Mesh Topology Dimension: " << TD << std::endl;
    for (int d = 0; d <= 3; ++d) {
        auto it = entity_storage.find(d);
        if (it != entity_storage.end())
            std::cout << "Entities of dim " << d << ": " << it->second.size() << std::endl;
    }
}

int MeshDS::estr2dim(const std::string& etype) const {
    if (etype == "node") return 0;
    if (etype == "edge") return 1;
    if (etype == "face") return 2;
    if (etype == "cell") return 3;
    throw std::invalid_argument("Unknown entity type: " + etype);
}

} // namespace XMFEM
