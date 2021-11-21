#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define UNRECHEABLE() assert(!"This line should be unrecheable.");

namespace caches {
template <typename Key_t, typename Val_t> class LRU_t {
public:
    LRU_t(const LRU_t<Key_t, Val_t> &) = delete;
    LRU_t(LRU_t<Key_t, Val_t> &&) = default;
    LRU_t(size_t size) : size_(size) {}

    template <typename F> bool look_update(Key_t key, F slow_path) {
        auto hit = hash_.find(key);

        if (hit == hash_.end()) {
            if (full()) {
                hash_.erase(cache_.back());
                cache_.pop_back();
            }
            cache_.push_front(std::move(slow_path(key)));
            hash_[key] = cache_.begin();
            return false;
        }

        auto eltit = hit->second;
        if (eltit != cache_.begin())
            cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));
        return true;
    }

    ~LRU_t() = default;

private:
    size_t size_;
    using ListIt_t = typename std::list<Val_t>::iterator;
    std::list<Val_t> cache_;
    std::unordered_map<Key_t, ListIt_t> hash_;

    bool full() { return cache_.size() == size_; }
};

template <typename Key_t, typename Val_t> class LFU_t {
public:
    LFU_t(const LRU_t<Key_t, Val_t> &) = delete;
    LFU_t(LFU_t<Key_t, Val_t> &&) = default;
    LFU_t(size_t size) : size_(size), min_freq_(1), n_elemets_(0) {}

    template <typename F> bool look_update(Key_t key, F slow_path) {
        if (hash_map_.find(key) != hash_map_.end()) {
            auto it = hash_map_[key];
            auto freq = it->freq++;

            freq_map_[freq + 1].push_front(std::move(*it));
            freq_map_[freq].erase(it);
            if (freq_map_[freq].empty()) {
                freq_map_.erase(freq);
                min_freq_ += freq == min_freq_;
            }

            hash_map_[key] = freq_map_[freq + 1].begin();

            return true;
        }

        if (n_elemets_ >= size_) {
            auto key = freq_map_[min_freq_].back().key;
            hash_map_.erase(key);
            freq_map_[min_freq_].pop_back();
            if (freq_map_[min_freq_].empty())
                freq_map_.erase(min_freq_);
            n_elemets_--;
        }

        n_elemets_++;
        min_freq_ = 1;
        freq_map_[min_freq_].push_front(std::move(Node_t{key, std::move(slow_path(key)), min_freq_}));
        hash_map_[key] = freq_map_[min_freq_].begin();

        return false;
    }

    void dump() const {
        for (auto &f : freq_map_) {
            std::cout << f.first << "\n";
            for (auto &it : f.second)
                std::cout << it.key << ", ";
            std::cout << std::endl;
        }
    }

    ~LFU_t() = default;

private:
    struct Node_t {
        Key_t key;
        Val_t val;
        size_t freq;
    };
    size_t size_;
    size_t min_freq_;
    size_t n_elemets_;

    using List_t = typename std::list<Node_t>;
    using ListIt_t = typename std::list<Node_t>::iterator;
    std::unordered_map<Key_t, ListIt_t> hash_map_;
    std::unordered_map<size_t, List_t> freq_map_;
};

template <typename Key_t, typename Val_t> class perfect_t {
public:
    perfect_t(const perfect_t<Key_t, Val_t> &) = delete;
    perfect_t(perfect_t<Key_t, Val_t> &&) = default;
    perfect_t(size_t size, std::vector<Key_t> &req) : size_(size), current_request_index(0) {
        hits_history_.resize(req.size());
        req_extend_.resize(req.size());

        // save key & it's index in sequence
        for (size_t i = 0; i < req.size(); i++)
            req_extend_[i] = triple{req[i], i, std::numeric_limits<size_t>::max()};

        misses_amount();
    }

    template <typename F> bool look_update(Key_t key, F slow_path) {
        if (current_request_index >= req_extend_.size()) {
            UNRECHEABLE();
            return false;
        }

        return hits_history_[current_request_index++];
    }

    size_t misses_amount() {
        // sorting by key value
        std::sort(req_extend_.begin(), req_extend_.end(), [](const triple &lhs, const triple &rhs) {
            bool is_same_key = lhs.key == rhs.key;
            return is_same_key ? lhs.idx < rhs.idx : lhs.key < rhs.key;
        });

        for (size_t i = 0; i + 1 < req_extend_.size(); i++) {
            if (req_extend_[i].key == req_extend_[i + 1].key) {
                req_extend_[i].next = req_extend_[i + 1].idx;
            }
        }

        // sorting by idx
        std::sort(req_extend_.begin(), req_extend_.end(),
                  [](const triple &lhs, const triple &rhs) { return lhs.idx < rhs.idx; });

        Cache_table_t cache_table;
        size_t N_iterations = req_extend_.size();
        size_t total_misses = 0;

        for (size_t i = 0; i < N_iterations; i++) {
            int cur_key = req_extend_[i].key;

            if (cache_table.count(cur_key) == 1) {
                cache_table[cur_key] = req_extend_[i].next;
                hits_history_[i] = 1;
                continue;
            }

            total_misses++;
            hits_history_[i] = 0;

#ifdef DONT_CACHE_SINGLES_PAGES
            if (req_extend_[i].next == std::numeric_limits<size_t>::max())
                continue;
#endif

            auto remove_key = cur_key;
            size_t dist = 0;
            for (const auto &p : cache_table) {
                if (p.second > dist) {
                    dist = p.second;
                    remove_key = p.first;
                }
            }

            if (size_ == cache_table.size())
                cache_table.erase(remove_key);

            cache_table[cur_key] = req_extend_[i].next;
        }

        return total_misses;
    }

    ~perfect_t() = default;

private:
    size_t size_;
    std::vector<bool> hits_history_;
    size_t current_request_index;
    using Cache_table_t = std::unordered_map<int, size_t>;

    struct triple {
        int key;
        size_t idx;
        size_t next;
    };
    std::vector<triple> req_extend_;
};

} // namespace caches
