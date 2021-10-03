#pragma once
#include <cstddef>
#include <cassert>
#include <list>
#include <vector>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stack>
#include <iostream>

#define UNRECHEABLE() assert(!"This line should be unrecheable.");

namespace caches
{
    class LRU_t{
        public:
            LRU_t(const LRU_t&) = delete;
            LRU_t(size_t size) : size_(size) {}
            
            bool look_update(int key);

            ~LRU_t() = default;
        private:
            size_t size_;
            using ListIt_t = typename std::list<int>::iterator;
            std::list<int> cache_;
            std::unordered_map<int, ListIt_t> hash_;

            bool full() { return cache_.size() == size_; }
    }; 

    template <typename Key_t, typename Val_t>
    class LFU_t {
        public:
            LFU_t(const LRU_t&) = delete;
            LFU_t(size_t size) :
                size_(size), min_freq_(1),
                n_elemets_(0)
            {}
            
            bool look_update(Key_t key) {
                if(hash_map_.find(key) != hash_map_.end()) {
                    auto it = hash_map_[key];
                    auto freq = it->freq++;

                    freq_map_[freq + 1].push_front(std::move(*it));
                    freq_map_[freq].erase(it);
                    if(freq_map_[freq].empty()) {
                        freq_map_.erase(freq);
                        min_freq_ += freq == min_freq_;
                    }

                    hash_map_[key] = freq_map_[freq + 1].begin();

                    return true;
                }

                if(n_elemets_ >= size_) {
                    auto key = freq_map_[min_freq_].back().key;
                    hash_map_.erase(key);
                    freq_map_[min_freq_].pop_back();
                    if(freq_map_[min_freq_].empty())
                        freq_map_.erase(min_freq_);
                    n_elemets_--;
                }

                n_elemets_++;
                min_freq_ = 1;
                freq_map_[min_freq_].push_front(std::move(Node_t{key, 0, min_freq_}));
                hash_map_[key] = freq_map_[min_freq_].begin();

                return false;
            }

            void dump() const {
                for(auto& f : freq_map_) {
                    std::cout << f.first << "\n";
                    for(auto& it : f.second)
                        std::cout << it.key << ", ";
                    std::cout<< std::endl;
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

    class perfect_t {
        public:
            perfect_t(const perfect_t&) = delete;
            perfect_t(size_t size, std::vector<int>& req) :
                size_(size),
                req_(req)
            {}
            
            bool look_update(int key);

            size_t misses_amount() const;

            ~perfect_t() = default;
        private:
            size_t size_;
            std::vector<int> req_;
            using Cache_table_t = std::unordered_set<int>;
            size_t misses_at_current_cache(const Cache_table_t& table, size_t from) const;
    };

} // namespace cache
