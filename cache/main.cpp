#include <iostream>
#include "cache.hpp"

#define emit(var) std::cout << #var " = " << var << std::endl;

static inline void print_test_title(size_t cur, size_t total, bool ok) {
    std::cout << "[" << cur << "/" << total << "] " << (ok ? "[PASSED]" : "[FAILED]\n");
    std::cout << (cur < total ? "\r" : "");
    std::cout << (ok && cur == total ? "\n" : "") << std::flush;
        
}

void test_perfect_cache() {
    struct Test_t {
        size_t N;
        std::vector<int> req;
        size_t answ;
    };

    Test_t tests[] = {
        {1, {1,1}, 1},
        {1, {1,2}, 2},
        {2, {1,1}, 1},
        {2, {1,2}, 2},
        {2, {1,1,2,2}, 2},
        {2, {1,1,3,2,2,2,1}, 3},
        {3, {1,1,3,2,2,2,1}, 3},
        {3, {1,2,3,4,3,3,3,5,5,5,5,1}, 5},
        {3, {1,2,3,4,4,4,5,5,5,7,6,6,6,6,6,1,4,4,4,4,5,5,5}, 8},
        {4, {1,2,3,4,  5,  1,2,3,4,  6,  1,2,3,4,  7,  1,2,3,4,  8,  1,2,3,4}, 8},
        {4, {1,2,3,4,  5,  1,2,3,4,  5,  1,2,3,4,  5,  1,2,3,4,  5,  1,2,3,4,  5, 5}, 9},
    };

    size_t N_tests = sizeof(tests)/sizeof(tests[0]);
    for(size_t i = 0; i < N_tests; i++) {
        caches::perfect_t cache(tests[i].N, tests[i].req);
        size_t answ = cache.hits_amount();
        if(answ != tests[i].answ) {
            print_test_title(i+1, N_tests, 0);
            std::cout << "Cache size = " << tests[i].N << std::endl;
            std::cout << "Requests = {";
            for(auto it = tests[i].req.begin(); it != tests[i].req.end(); it++)
                std::cout << *it << (std::next(it) == tests[i].req.end() ? "" : ", ");
            std::cout << "};" << std::endl;
            std::cout << "Hits got      = " << answ << std::endl
                      << "Hits expected = " << tests[i].answ << std::endl;     
            return;
        }
        print_test_title(i+1, N_tests, 1);
    }
}

int main() {
    test_perfect_cache();
    return 0;
}