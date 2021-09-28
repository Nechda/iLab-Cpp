#include "cache.hpp"

#if 0
namespace caches
{

    bool LFU_t::look_update(int key) {
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
}
#endif