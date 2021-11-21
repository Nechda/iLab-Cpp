#include "Tree.hpp"
#include <vector>
#include <set>
#include <random>
#include <iomanip>
#include <chrono>

using namespace Containers;

std::vector<int> get_random_array(size_t N, int range_radius) {
    std::random_device rd;
    std::default_random_engine el(rd());
    std::uniform_int_distribution<int> uniform_dist(-range_radius, range_radius);

    std::vector<int> arr(N);
    for(int i = 0; i < arr.size(); i++)
        arr[i] = uniform_dist(el);
    
    return arr;
}

bool comare_vectors(std::vector<int>& a, std::vector<int>& b) {
    auto size = a.size();
    if(size != b.size())
        std::cout << "\n[Fail] Different sizes" << std::endl;

    for(int i = 0; i < size; i++) {
        if(a[i] != b[i]) {
            std::cout << "\nIndex = " << i << std::endl;
            std::cout << "a[i] = " << a[i] << std::endl;
            std::cout << "b[i] = " << b[i] << std::endl;

            std::cout << "{Dump} a:" << std::endl;
            for(auto it : a)
                std::cout << std::setw(4) << it << " ";
            std::cout << std::endl;

            std::cout << "{Dump} b:" << std::endl;
            for(auto it : b)
                std::cout << std::setw(4) << it << " ";
            std::cout << std::endl;

            return 0;
        }
    }

    std::cout << "Ok ";

    return 1;
}

void t_tree() {
    const size_t N_tests = 1000;
    const ssize_t RANGE = 5000;
    auto sizes = get_random_array(N_tests, RANGE);
    for(const auto& size : sizes) {
        const size_t array_size = size + RANGE + 1;
        auto arr = get_random_array(array_size, array_size * 3);

        std::set<int> s(arr.begin(), arr.end());

        arr.clear();
        {
            Tree t;
            for(auto it : s) {
                t.insert(it);
                arr.push_back(it);
            }
            std::vector<int> from_tree(s.size());

            for(int i = 1; i <= s.size(); i++) {
                auto it = t.nth(i);
                from_tree[i-1] = it;
            }

            bool is_pass = comare_vectors(arr, from_tree);
            if(!is_pass)
                return;
        }
    }
}

int main() {
    t_tree();
    return 0;
}
