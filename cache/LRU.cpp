#include "cache.hpp"

namespace caches
{
    bool cache_t::look_update(int key) {
        auto hit = hash_.find(key);
        
        if (hit == hash_.end()) {
            if (full()) {
                hash_.erase(cache_.back());
                cache_.pop_back();
            }
            cache_.push_front(key);
            hash_[key] = cache_.begin();
            return false;
        }
        
        auto eltit = hit->second;
        if (eltit != cache_.begin())
            cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));
        return true;
    }
}