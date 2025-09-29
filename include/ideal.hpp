#pragma once

#include <limits>
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>

/// @file Header-only imlpementation of ideal cache algorithm

/**
 * @brief Ideal cache (Beladi's algorithm)
 *
 * @tparam TKey key that is used to address cache pages (usually int or string)
 * Simulates ideal cache where future is known beforehand
 * Replacement policy: remove page that will be requested later than others
 */
template<typename TKey>
class IdealCache {
public:
    IdealCache(std::size_t cache_size): requests(), sz_(cache_size) {}

    void lookup_update(TKey key) {
        requests.push_back(key);
    }

    std::size_t get_hits() const;
private:
    std::vector<TKey> requests;
    std::size_t sz_;
};

template <typename TKey>
std::size_t IdealCache<TKey>::get_hits() const {
    const unsigned INFTY_TIME = std::numeric_limits<unsigned>().max();

    if (requests.empty()) return 0;

    // ------------------------ PRECALCULATIONS -------------------------
    // Storing request times for every page in reversed order
    std::unordered_map<TKey, std::vector<unsigned>> page_request_times;
    for (long time = (long) (requests.size() - 1); time >= 0; time--) {
        page_request_times[requests[(std::size_t)time]].push_back((unsigned)time);
    }

    std::map<unsigned, TKey> cache_rev;
    std::unordered_map<TKey, unsigned> cache_fwd;

    std::size_t hits = 0;
    // ------------------------- SIMULATING REQUESTS --------------------------
    for (unsigned time = 0; time < requests.size(); time++) {
        TKey key = requests[time];
        std::vector<unsigned>& key_requests = page_request_times[key];
        unsigned next_seen_time = INFTY_TIME;
        key_requests.pop_back();
        if (!key_requests.empty()) {
            next_seen_time = key_requests.back();
        }

        if (cache_fwd.contains(key))
        {
            // ---------------------- HIT --------------------------------
            hits++;
            if (next_seen_time != INFTY_TIME) {
                unsigned old_time = cache_fwd[key];
                cache_fwd[key] = next_seen_time;
                cache_rev.erase(old_time);
                cache_rev[next_seen_time] = key;
            } else {
            // this key will not be requested in the future, so it is erased
                cache_fwd.erase(key);
                cache_rev.erase(time);
            }
            continue;
        } else
        {
            // --------------------- MISS ---------------------------------
            if (next_seen_time == INFTY_TIME) {
                continue;
            } else  {
                // adding new page first
                cache_fwd[key] = next_seen_time;
                cache_rev[next_seen_time] = key;
                // removing page that will be requested further in the future
                if (cache_fwd.size() > sz_) {
                    // using map as O(log n) sorter
                    const TKey &key_to_delete = std::prev(cache_rev.end())->second;
                    cache_fwd.erase(key_to_delete);
                    cache_rev.erase(std::prev(cache_rev.end()));
                }
            }
        }

    }

    return hits;
}
