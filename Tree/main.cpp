#include "Tree.hpp"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <random>
#include <set>
#include <sstream>
#include <vector>

using namespace Containers;

Containers::tree_container::FlatStack Containers::tree_container::stack_s0_;
Containers::tree_container::FlatStack Containers::tree_container::stack_s1_;

std::vector<int> get_random_array(size_t N, int range_radius) {
    std::random_device rd;
    std::default_random_engine el(rd());
    std::uniform_int_distribution<int> uniform_dist(-range_radius, range_radius);

    std::vector<int> arr(N);
    for (size_t i = 0; i < arr.size(); i++)
        arr[i] = uniform_dist(el);

    return arr;
}

bool comare_vectors(std::vector<int> &a, std::vector<int> &b) {
    auto size = a.size();
    if (size != b.size())
        std::cout << "\n[Fail] Different sizes" << std::endl;

    for (size_t i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            std::cout << "\nIndex = " << i << std::endl;
            std::cout << "a[i] = " << a[i] << std::endl;
            std::cout << "b[i] = " << b[i] << std::endl;

            std::cout << "{Dump} a:" << std::endl;
            for (auto it : a)
                std::cout << std::setw(4) << it << " ";
            std::cout << std::endl;

            std::cout << "{Dump} b:" << std::endl;
            for (auto it : b)
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
    for (const auto &size : sizes) {
        const size_t array_size = size + RANGE + 1;
        auto arr = get_random_array(array_size, array_size * 3);

        std::set<int> s(arr.begin(), arr.end());

        arr.clear();
        {
            Tree t;
            for (auto it : s) {
                t.insert(it);
                arr.push_back(it);
            }
            std::vector<int> from_tree(s.size());

            for (size_t i = 1; i <= s.size(); i++) {
                auto it = t.nth(i);
                from_tree[i - 1] = it;
            }

            bool is_pass = comare_vectors(arr, from_tree);
            if (!is_pass)
                return;
        }
    }
}

int t_tree_e2e(int argc, char **argv) {
    bool quiet = 0;
    for (int i = 0; i < argc && !quiet; i++)
        quiet |= !std::strcmp("--quiet", argv[i]);

    std::string in, out;
    std::getline(std::cin, in);
    std::getline(std::cin, out);
    std::stringstream ss_in(in);
    std::stringstream ss_out(out);
    std::stringstream ss_answ;
    Tree tree;
    do {
        char req = 0;
        int arg = 0;
        ss_in >> req >> arg;
        switch (req) {
        case 'k':
            tree.insert(arg);
            break;
        case 'm':
            ss_answ << tree.nth(arg) << " ";
            break;
        case 'n':
            ss_answ << tree.upper_bound(arg) << " ";
            break;
        }
    } while (ss_in);

    if (!quiet) {
        std::cout << "My answer : " << std::endl;
        std::cout << ss_answ.str() << std::endl;
        std::cout << "Correct answer: " << std::endl;
        std::cout << ss_out.str() << std::endl;
        tree.dump();
    }

    bool pass = 1;
    int idx = 1;
    while (ss_out && pass) {
        int correct = 0;
        int answ = 0;
        ss_out >> correct;
        ss_answ >> answ;
        pass &= correct == answ;
        if (!quiet) {
            std::cout << "[" << idx << "]: " << correct << " vs " << answ << std::endl;
            idx++;
        }
    }

    std::cout << (pass ? "Ok" : "Failed") << std::endl;

    return !pass;
}

void tree_eval() {
    Tree tree;
    do {
        char req = 0;
        int arg = 0;
        std::cin >> req >> arg;
        switch (req) {
        case 'k':
            tree.insert(arg);
            break;
        case 'm':
            std::cout << tree.nth(arg) << std::endl;
            break;
        case 'n':
            std::cout << tree.upper_bound(arg) << std::endl;
            break;
        }
    } while (std::cin);
}

int main(int argc, char **argv) {

#ifdef E2ETESTER
    return t_tree_e2e(argc, argv);
#endif

#ifdef STRESS
    t_tree();
#endif

#ifdef UI
    tree_eval();
#endif
    return 0;
}
