#include "cache.hpp"

namespace caches
{
    bool LFU_t::look_update(int key) {

        auto comp = [&](const DataIdx_t a, const DataIdx_t b) {
            return data_[a].freq > data_[b].freq;
        };

        if(hash_.find(key) != hash_.end()) {
            auto idx = hash_[key];
            auto& node = data_[idx];
            node.freq++;
            heapify(data_, size_, idx, comp);
            return true;
        }

        // just insert
        if(n_elemets_ == size_) {
            // but before remove least frequent
            auto rm_idx = cache_[0];

            hash_.erase(data_[rm_idx].key);
            allocator_.free(rm_idx);
            cache_[0] = cache_.back();
            cache_.pop_back();

            heapify(data_, size_ - 1, 0, comp);
            n_elemets_--;
        }

        n_elemets_++;
        auto isrt_idx = allocator_.alloc();
        hash_[key] = isrt_idx;
        cache_.push_back(isrt_idx);
        data_[isrt_idx] = Node_t{key, 0, 1};
        
        size_t i = 0;
        auto parent = [&](size_t idx) { return (idx-1) >> 1; };
        while(i ? comp(parent(i), i) : 0) {
            std::swap(data_[i], data_[parent(i)]);
            i = parent(i);
        }
        
        return false;
    }
}