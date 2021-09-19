#pragma once
#include <cstddef>
#include <cassert>
#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#define UNRECHEABLE() assert("This line should be unrecheable.");

namespace caches
{
    class ICache {
        public:
            ICache() = delete;
            ICache(size_t sz) : size_(sz) {};
            virtual bool look_update(int key) = 0;
            ~ICache() = default;
        protected:
            size_t size_;
    };


    class LRU_t : public ICache {
        public:
            LRU_t() = delete;
            LRU_t(const LRU_t&) = delete;
            LRU_t(size_t size) : ICache(size) {}
            
            bool look_update(int key) override;

            ~LRU_t() = default;
        private:
            using ListIt_t = typename std::list<int>::iterator;
            std::list<int> cache_;
            std::unordered_map<int, ListIt_t> hash_;

            bool full() { return cache_.size() == size_; }
    }; 

    class LFU_t : public ICache {
        public:
            LFU_t() = delete;
            LFU_t(const LRU_t&) = delete;
            LFU_t(size_t size) :
                ICache(size),
                min_freq_(1), n_elemets_(0) 
            {}
            
            bool look_update(int key) override;

            ~LFU_t() = default;
        private:
            struct Node_t {
                int key;
                int val;
                size_t freq;
            };
            using List_t = typename std::list<Node_t>;
            using ListIt_t = typename List_t::iterator;
            std::unordered_map<int, ListIt_t> cache_;
            std::unordered_map<int, List_t> freq_map_;

            size_t min_freq_ = 1;
            size_t n_elemets_ = 0;

            void update_freq(int key);
    }; 

    class perfect_t : public ICache {
        public:
            perfect_t() = delete;
            perfect_t(const perfect_t&) = delete;
            perfect_t(size_t size, std::vector<int>& req) :
                ICache(size),
                req_(req)
            {}
            
            bool look_update(int key) override;

            size_t hits_amount() const;

            ~perfect_t() = default;
        private:
            using Cache_table_t = std::unordered_set<int>;
            std::vector<int> req_;
            size_t misses_at_current_cache(const Cache_table_t& table, size_t from) const;
    };

} // namespace cache