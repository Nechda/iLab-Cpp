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
                ICache(size), allocator_(size),
                min_freq_(1), n_elemets_(0), data_(size)
            {}
            
            bool look_update(int key) override;

            ~LFU_t() = default;
        private:
            struct Node_t {
                int key;
                int val;
                size_t freq;
            };

            class Allocator {
                public:
                    Allocator() = delete;
                    Allocator(const Allocator&) = delete;
                    Allocator(size_t sz) : size_(sz) {
                        for(size_t i = 0; i < sz; i++)
                            available.push(i);
                    }
                    
                    size_t alloc() {
                        if(available.empty())
                            return -1;
                        
                        auto idx = available.top();
                        available.pop();
                        return idx;
                    }

                    void free(size_t idx) {
                        available.push(idx);
                    }

                    ~Allocator() = default;
                private:
                    size_t size_;
                    std::stack<size_t> available;
                    
            };
            using DataIdx_t = size_t;

            std::vector<DataIdx_t> cache_;
            std::unordered_map<int, DataIdx_t> hash_;

            size_t min_freq_ = 1;
            size_t n_elemets_ = 0;

            Allocator allocator_;
            std::vector<Node_t> data_;

            template<typename T, typename F>
            void heapify(std::vector<T> arr, int n, int i, F cmp)
            {
                int largest = i;
                int l = 2 * i + 1;
                int r = 2 * i + 2;
            
                if (l < n && cmp(l,largest))
                    largest = l;
            
                if (r < n && cmp(r,largest))
                    largest = r;
            
                if (largest != i) {
                    std::swap(arr[i], arr[largest]);
                    heapify(arr, n, largest, cmp);
                }
            }
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