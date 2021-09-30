#include "octree.hpp"
#include "triangle.hpp"
#include <iostream>
#include <vector>

using namespace Geomentry;

void print(const Vec3 &v) {
    std::cout << v.x << ", " << v.y << ", " << v.z << std::endl;
}

void print(const Triangle &t) {
    std::cout << "Triangle {\n";
    print(t[0]);
    std::cout << std::endl;
    print(t[1]);
    std::cout << std::endl;
    print(t[2]);
    std::cout << std::endl;
    std::cout << "}\n";
}

void tr_intersection_tests() {
    size_t N = 2;
    std::vector<Geomentry::Triangle> trs(N);
    for (auto &tr : trs) {
        for (auto v : {0, 1, 2})
            for (auto d : {0, 1, 2})
                std::cin >> tr[v][d];
    }

    bool res = Geomentry::is_intersected_impl(trs[0], trs[1]);
    bool ans = 0;
    std::cin >> ans;
    if (ans != res)
        std::cout << "Failed\n";
    else
        std::cout << "Ok\n";
}

int main() {

    // tr_intersection_tests();
    // return 0;

    // ввод данных
    size_t N = 0;
    std::cin >> N;
    std::vector<Geomentry::Triangle> trs(N);
    for (auto &tr : trs) {
        for (auto v : {0, 1, 2})
            for (auto d : {0, 1, 2})
                std::cin >> tr[v][d];
    }

    // поиск области, в которой лежат треугольники
    Geomentry::Vec3 min;
    Geomentry::Vec3 max;
    for (auto &tr : trs) {
        for (auto d : {0, 1, 2})
            for (auto v : {0, 1, 2}) {
                min[d] = std::min(min[d], tr[v][d]);
                max[d] = std::max(max[d], tr[v][d]);
            }
    }

    // print(min);
    // print(max);

    // закидываем в дерево октантов треугольники
    Algorithm::Octree tree(min, max, trs);
    for (size_t i = 0; i < N; i++)
        tree.insert(trs[i], i);

    // tree.dump();

    // проходимя по нему через dfs
    tree.DFS();

    // std::cout << "Intersected triangles {" << std::endl;
    auto set = tree.get_set();
    for (const auto &it : set)
        std::cout << it << " ";
    // std::cout << "}" << std::endl;

    return 0;
}
