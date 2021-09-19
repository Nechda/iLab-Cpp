#include "cache.hpp"

namespace caches
{
    bool LFU_t::look_update(int key) {
        const int value = 0;
        // if key is in hash table already
        // then just update its frequent
        if(cache_.find(key) != cache_.end()) {
            auto& it = cache_[key];
            it->val = value;
            update_freq(key);
            return true;
        }

        // if we dont have any space, so remove 
        // item from with the lowest frequency
        if(n_elemets_ >= size_) {
            auto& lowest_freq_list = freq_map_[min_freq_];
            auto& it = lowest_freq_list.back();
            cache_.erase(it.key);
            lowest_freq_list.pop_back();
            if(lowest_freq_list.empty())
                freq_map_.erase(min_freq_);
            n_elemets_--;
        }

        // overwise insert the element into cache
        min_freq_ = 1;
        auto& lowest_freq_list = freq_map_[min_freq_];
        auto node = Node_t{key, value, min_freq_};
        lowest_freq_list.push_front(node);
        cache_[key] = lowest_freq_list.begin();
        n_elemets_++;
        
        return false;
    }

    void LFU_t::update_freq(int key) {
        if(cache_.find(key) == cache_.end())
            return;
        
        // reconstruct frequencies table
        auto& it = cache_[key];
        auto node = *it;
        freq_map_[node.freq].erase(it);
        if(freq_map_[node.freq].empty()) {
            freq_map_.erase(node.freq);
            min_freq_ += node.freq == min_freq_;
        }

        // save new iterator in hash map
        node.freq++;
        freq_map_[node.freq].push_front(node);
        cache_[key] = freq_map_[node.freq].begin();
    }
}