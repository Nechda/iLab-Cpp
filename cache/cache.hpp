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
    template <typename Key_t, typename Val_t>
    class LRU_t{
        public:
            LRU_t(const LRU_t<Key_t, Val_t>&) = delete;
            LRU_t(size_t size) : size_(size) {}
            
            bool look_update(Key_t key, Val_t&& val = Val_t()) {
                auto hit = hash_.find(key);

                if (hit == hash_.end()) {
                    if (full()) {
                        hash_.erase(cache_.back());
                        cache_.pop_back();
                    }
                    cache_.push_front(std::move(val));
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

    template <typename Key_t, typename Val_t>
    class LFU_t {
        public:
            LFU_t(const LRU_t<Key_t, Val_t>&) = delete;
            LFU_t(size_t size) :
                size_(size), min_freq_(1),
                n_elemets_(0)
            {}
            
            bool look_update(Key_t key, Val_t&& val = Val_t()) {
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
                freq_map_[min_freq_].push_front(std::move(Node_t{key, std::move(val), min_freq_}));
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

    template <typename Key_t, typename Val_t>
    class perfect_t {
        public:
            perfect_t(const perfect_t<Key_t, Val_t>&) = delete;
            perfect_t(size_t size, std::vector<Key_t>& req) :
                size_(size),
                req_(req)
            {}
            
            bool look_update(Key_t key, Val_t&& val = Val_t()) {
                UNRECHEABLE();
            }

            size_t misses_amount() const {
                Cache_table_t cache_table;
                size_t N_iterations = req_.size();
                size_t total_misses = 0;
                std::vector<int> cached_keys(size_);
                cached_keys.resize(0);

                for(size_t i = 0; i < N_iterations; i++) {
                    int cur_key = req_[i];
                    if(cache_table.count(cur_key) == 1)
                        continue;
                    total_misses++;
                    size_t min_misses = -1;
                    size_t tmp_misses = -1;

                    // without cache new element
                    tmp_misses = misses_at_current_cache(cache_table, i);
                    min_misses = std::min(min_misses, tmp_misses);

                    // transform unordered_set into vector
                    cached_keys.resize(0);
                    for(const auto& it : cache_table)
                        cached_keys.push_back(it);
                    
                    // if there is free space just cache a new key
                    if(cached_keys.size() < size_) {
                        cache_table.insert(cur_key);
                        continue;
                    }

                    // find the most gainful place in cache table
                    int replaced_key = -1;
                    for(auto key : cached_keys) {
                        cache_table.erase(key);
                        cache_table.insert(cur_key);
                        
                        tmp_misses = misses_at_current_cache(cache_table, i);
                        if(tmp_misses < min_misses)
                            replaced_key = key;
                        min_misses = std::min(min_misses, tmp_misses);
                        
                        cache_table.erase(cur_key);
                        cache_table.insert(key);
                    }

                    // overwise replace item in the cache table
                    if(replaced_key != -1) {
                        cache_table.erase(replaced_key);
                        cache_table.insert(cur_key);
                        continue;
                    }

                }
                return total_misses;
            }

            ~perfect_t() = default;
        private:
            size_t size_;
            std::vector<int> req_;
            using Cache_table_t = std::unordered_set<int>;
            size_t misses_at_current_cache(const Cache_table_t& table, size_t from) const {
                size_t hits = 0;
                size_t N = req_.size();
                for(size_t i = from; i < N; i++)
                    hits += !table.count(req_[i]);
                return hits;
            }
    };

} // namespace cache
