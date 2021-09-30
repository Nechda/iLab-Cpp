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

int tr_intersection_tests() {
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

    return ans != res;
}

int main() {
#ifdef INTERSECTED_FUNCTION_TEST
    return tr_intersection_tests();;
#else
    // take data from stdin
    size_t N = 0;
    std::cin >> N;
    std::vector<Geomentry::Triangle> trs(N);
    for (auto &tr : trs) {
        for (auto v : {0, 1, 2})
            for (auto d : {0, 1, 2})
                std::cin >> tr[v][d];
    }

    // find an area where located all triangles
    Geomentry::Vec3 min;
    Geomentry::Vec3 max;
    for (auto &tr : trs) {
        for (auto d : {0, 1, 2})
            for (auto v : {0, 1, 2}) {
                min[d] = std::min(min[d], tr[v][d]);
                max[d] = std::max(max[d], tr[v][d]);
            }
    }

    // create tree
    Algorithm::Octree tree(min, max, trs);
    for (size_t i = 0; i < N; i++)
        tree.insert(trs[i], i);

    // dfs
    tree.DFS();

    auto set = tree.get_set();
    for (const auto &it : set)
        std::cout << it << " ";

    return 0;
#endif
}
