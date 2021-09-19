#pragma once
#include <cstddef>
#include <cassert>
#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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


    class cache_t : public ICache {
        public:
            cache_t() = delete;
            cache_t(const cache_t&) = delete;
            cache_t(size_t size) : ICache(size) {}
            
            bool look_update(int key) override;

            ~cache_t() = default;
        private:
            using ListIt_t = typename std::list<int>::iterator;
            std::list<int> cache_;
            std::unordered_map<int, ListIt_t> hash_;

            bool full() { return cache_.size() == size_; }
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
            size_t hits_at_current_cache(const Cache_table_t& table, size_t from) const;
    };

} // namespace cache