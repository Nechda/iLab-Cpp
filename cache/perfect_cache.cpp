#include "cache.hpp"

namespace caches
{
    bool perfect_t::look_update(int key)
    {
        UNRECHEABLE();
        return false;
    }

    size_t perfect_t::hits_at_current_cache(const Cache_table_t& table, size_t from) const {
        size_t hits = 0;
        size_t N = req_.size();
        for(size_t i = from; i < N; i++)
            hits += !table.count(req_[i]);
        return hits;
    }

    size_t perfect_t::hits_amount() const{
        Cache_table_t cache_table;
        size_t N_iterations = req_.size();
        size_t total_hits = 0;
        std::vector<int> cached_keys(size_);
        cached_keys.resize(0);

        for(size_t i = 0; i < N_iterations; i++) {
            int cur_key = req_[i];
            if(cache_table.count(cur_key) == 1)
                continue;
            total_hits++;
            size_t min_hits = -1;
            size_t tmp_hits = -1;

            // without cache new element
            tmp_hits = hits_at_current_cache(cache_table, i);
            min_hits = std::min(min_hits, tmp_hits);

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
                
                tmp_hits = hits_at_current_cache(cache_table, i);
                if(tmp_hits < min_hits)
                    replaced_key = key;
                min_hits = std::min(min_hits, tmp_hits);
                
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
        return total_hits;
    }
}