#include <iostream>
#include "Octree/octree.hpp"
#include "Octree/vector.hpp"
#include "Octree/triangle.hpp"

int main() {
    size_t N = 0;
    size_t total_time = 0;
    std::cin >> N >> total_time;
    struct Pack_of_data {
        glm::Triangle tr;
        glm::vec3 axis[2];
        double omega;
    };
    std::vector<Pack_of_data> data(N);
    for (auto& it : data) {
        std::cin >> it.tr;
        std::cin >> it.axis[0] >> it.axis[1];
        std::cin >> it.omega;
    }

    std::vector<Geomentry::Cylinder> cylinders(N);
    for (size_t i = 0; i < N; i++) {
        cylinders[i] = Geomentry::Cylinder(data[i].tr, { data[i].axis[0], data[i].axis[1] });
    }

    Geomentry::Vec3 min = cylinders[0].get_box().get_min();
    Geomentry::Vec3 max = cylinders[0].get_box().get_max();
    for (size_t i = 1; i < N; i++) {
        auto min_ = cylinders[i].get_box().get_min();
        auto max_ = cylinders[i].get_box().get_max();
        
        for (int i = 0; i < 3; i++) {
            min[i] = std::min(min[i], min_[i]);
            max[i] = std::max(max[i], max_[i]);
        }
    }

    Algorithm::Octree tree(min, max, cylinders);
    for (size_t i = 0; i < N; i++)
        tree.insert(cylinders[i], i);

    tree.DFS();
    auto set_of_pairs = tree.get_set();
    

    std::cin >> N;
    decltype(set_of_pairs) actual;
    for (size_t i = 0; i < N; i++) {
        size_t a, b;
        std::cin >> a >> b;
        actual.insert({ a, b });
    }

    bool passed = actual == set_of_pairs;
    if (passed) {
        std::cout << "Ok" << std::endl;
        return 0;
    }

    std::cout << "[Failed]\n My set of pairs:" << std::endl;
    for (const auto& pp : set_of_pairs) {
        std::cout << "{" << pp.first << ", " << pp.second << "} ";
    }
    std::cout << "\n Expected set of pairs:" << std::endl;
    for (const auto& pp : actual) {
        std::cout << "{" << pp.first << ", " << pp.second << "} ";
    }
    return -1;
}
