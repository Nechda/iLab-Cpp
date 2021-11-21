#include "cache.hpp"
#include <iomanip>
#include <iostream>

#define emit(var) std::cout << #var " = " << var << std::endl;

using namespace std;

static inline void print_test_title(size_t cur, size_t total, bool ok) {
    std::cout << "[" << cur << "/" << total << "] " << (ok ? "[PASSED]" : "[FAILED]\n");
    std::cout << (cur < total ? "\r" : "");
    std::cout << (ok && cur == total ? "\n" : "") << std::flush;
}

struct Test_t {
    size_t N;
    std::vector<int> req;
    size_t answ;
};

template <typename Cache_t> struct Make_cache {};

template <typename K, typename V> struct Make_cache<caches::perfect_t<K, V>> {
    static caches::perfect_t<K, V> get(Test_t &t) { return caches::perfect_t<K, V>(t.N, t.req); }
};

template <typename K, typename V> struct Make_cache<caches::LFU_t<K, V>> {
    static caches::LFU_t<K, V> get(Test_t &t) { return caches::LFU_t<K, V>(t.N); }
};

template <typename Cache_t, size_t N_tests> void test_cache(Test_t *tests, const char *title_msg) {
    std::cout << title_msg << std::endl;
    auto slow_path = [](int key) -> int { return key; };

    for (size_t i = 0; i < N_tests; i++) {
        Cache_t cache = Make_cache<Cache_t>::get(tests[i]);
        size_t answ = 0;
        for (const auto &it : tests[i].req) {
            bool hit = cache.look_update(it, slow_path);
            answ += !hit;
        }
        if (answ != tests[i].answ) {
            print_test_title(i + 1, N_tests, 0);
            std::cout << "Cache size = " << tests[i].N << std::endl;
            std::cout << "Requests = {";
            for (auto it = tests[i].req.begin(); it != tests[i].req.end(); it++)
                std::cout << *it << (std::next(it) == tests[i].req.end() ? "" : ", ");
            std::cout << "};" << std::endl;
            std::cout << "Misses got      = " << answ << std::endl
                      << "Misses expected = " << tests[i].answ << std::endl;
            return;
        }
        print_test_title(i + 1, N_tests, 1);
    }
}

void test_caches() {
    Test_t tests_perfect[] = {
        {1, {1, 1}, 1},
        {1, {1, 2}, 2},
        {2, {1, 1}, 1},
        {2, {1, 2}, 2},
        {2, {1, 1, 2, 2}, 2},
        {2, {1, 1, 3, 2, 2, 2, 1}, 3},
        {3, {1, 1, 3, 2, 2, 2, 1}, 3},
        {3, {1, 2, 3, 4, 3, 3, 3, 5, 5, 5, 5, 1}, 5},
        {3, {1, 2, 3, 4, 4, 4, 5, 5, 5, 7, 6, 6, 6, 6, 6, 1, 4, 4, 4, 4, 5, 5, 5}, 8},
        {4, {1, 2, 3, 4, 5, 1, 2, 3, 4, 6, 1, 2, 3, 4, 7, 1, 2, 3, 4, 8, 1, 2, 3, 4}, 8},
        {4, {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 5}, 9},
    };
    test_cache<caches::perfect_t<int, int>, sizeof(tests_perfect) / sizeof(Test_t)>(tests_perfect,
                                                                                    "Perfect cache testing");

    Test_t tests_LFU[] = {{1, {1, 1}, 1},
                          {1, {1, 2}, 2},
                          {2, {1, 1}, 1},
                          {2, {1, 2}, 2},
                          {2, {1, 1, 2, 2}, 2},
                          {2, {1, 1, 3, 2, 2, 2, 1}, 3},
                          {3, {1, 1, 3, 2, 2, 2, 1}, 3},
                          {3, {1, 2, 3, 4, 3, 3, 3, 5, 5, 5, 5, 1}, 6},
                          {4, {1, 2, 1, 3, 2, 4, 5}, 5},
                          {3, {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2}, 9}};
    test_cache<caches::LFU_t<int, int>, sizeof(tests_LFU) / sizeof(Test_t)>(tests_LFU, "LFU cache testing");
}

void cache_comparison() {

    Test_t tests[] = {
        {1, {1, 1}},
        {1, {1, 2}},
        {2, {1, 1}},
        {2, {1, 2}},
        {2, {1, 1, 2, 2}},
        {2, {1, 1, 3, 2, 2, 2, 1}},
        {3, {1, 1, 3, 2, 2, 2, 1}},
        {3, {1, 2, 3, 4, 3, 3, 3, 5, 5, 5, 5, 1}},
        {3, {1, 2, 3, 4, 4, 4, 5, 5, 5, 7, 6, 6, 6, 6, 6, 1, 4, 4, 4, 4, 5, 5, 5}},
        {4, {1, 2, 3, 4, 5, 1, 2, 3, 4, 6, 1, 2, 3, 4, 7, 1, 2, 3, 4, 8, 1, 2, 3, 4}},
        {4, {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 5}},
    };

    std::cout << "Cache comparison (misses)" << std::endl;
    size_t N_tests = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < N_tests; i++) {
        caches::perfect_t<int, int> cache_0(tests[i].N, tests[i].req);
        caches::LFU_t<int, int> cache_1(tests[i].N);

        size_t misses_0 = 0;
        size_t misses_1 = 0;
        auto slow_path = [](int key) -> int { return key; };
        for (const auto &it : tests[i].req) {
            misses_0 += !cache_0.look_update(it, slow_path);
            misses_1 += !cache_1.look_update(it, slow_path);
        }

        std::cout << std::setw(8) << "perfect = " << std::setw(3) << misses_0 << std::setw(8)
                  << "LFU = " << std::setw(3) << misses_1 << std::endl;
    }
}

int main() {
#ifdef TESTS
    test_caches();
    cache_comparison();
#elif LFU
    size_t cache_size = 0;
    size_t req_amount = 0;
    std::cin >> cache_size >> req_amount;
    caches::LFU_t<int, int> cache(cache_size);
    auto slow_path = [](int key) -> int { return key; };

    size_t hits = 0;
    for (size_t i = 0; i < req_amount; i++) {
        int req = 0;
        std::cin >> req;
        hits += cache.look_update(req, slow_path);
    }
    std::cout << hits << std::endl;
#elif PERFECT
    size_t cache_size = 0;
    size_t req_amount = 0;
    std::cin >> cache_size >> req_amount;
    std::vector<int> req(req_amount);
    for (auto &r : req)
        std::cin >> r;
    caches::perfect_t<int, int> cache(cache_size, req);
    size_t hits = req_amount - cache.misses_amount();
    std::cout << hits << std::endl;
#endif
    return 0;
}
