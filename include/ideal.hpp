#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>

/// @file Header-only imlpementation of ideal cache algorithm

using std::size_t;
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
    IdealCache(size_t cache_size): sz_(cache_size), requests() {
        requests.reserve(10);
    }

    void lookup_update(TKey key) {
        requests.push_back(key);
    }

    size_t get_hits() const;
private:
    std::vector<TKey> requests;
    size_t sz_;
};

template <typename TKey>
size_t IdealCache<TKey>::get_hits() const {
    const int INFTY = 1e9;
    if (requests.size() >= INFTY) {
        std::cerr << "Too many requests, terminating\n";
        exit(5);
    } else if (requests.size() == 0) return 0;

    // ------------------------ PRECALCULATIONS -------------------------
    // Storing request times for every page in reversed order
    std::unordered_map<TKey, std::vector<int>> page_request_times;
    for (int time = (int) (requests.size() - 1); time >= 0; time--) {
        page_request_times[requests[(size_t)time]].push_back(time);
    }

    std::map<int, TKey> cache_rev;
    std::unordered_map<TKey, int> cache_fwd;

    size_t hits = 0;
    // ------------------------- SIMULATING REQUESTS --------------------------
    for (size_t time = 0; time < requests.size(); time++) {
        TKey key = requests[time];
        std::vector<int>& key_requests = page_request_times[key];
        int next_seen_time = INFTY;
        key_requests.pop_back();
        if (!key_requests.empty()) {
            next_seen_time = key_requests.back();
        }

        if (cache_fwd.contains(key))
        {
            // ---------------------- HIT --------------------------------
            hits++;
            if (next_seen_time != INFTY) {
                int old_time = cache_fwd[key];
                cache_fwd[key] = next_seen_time;
                cache_rev.erase(old_time);
                cache_rev[next_seen_time] = key;
            } else {
            // this key will not be requested in the future, so it is erased
                cache_fwd.erase(key);
                cache_rev.erase((int)time);
            }
            continue;
        } else
        {
            // --------------------- MISS ---------------------------------
            if (next_seen_time == INFTY) {
                continue;
            } else  {
                // adding new page first
                cache_fwd[key] = next_seen_time;
                cache_rev[next_seen_time] = key;
                // removing page that will be requested further in the future
                if (cache_fwd.size() > sz_) {
                    // using map as O(log n) sorter
                    TKey key_to_delete = std::prev(cache_rev.end())->second;
                    cache_rev.erase(std::prev(cache_rev.end()));
                    cache_fwd.erase(key_to_delete);
                }
            }
        }

    }

    return hits;
}
