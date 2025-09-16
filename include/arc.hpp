#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>

/// @file Header-only imlpementation of ARC and beladi (ideal) cache algorithms


/**
 * @brief Adaptive replacement cache
 *
 * @tparam TPage data that is stored in cache
 * @tparam TKey key that is used to address cache pages (usually int or string)
 * algo source: https://www2.cs.uh.edu/~paris/7360/PAPERS03/arcfast.pdf
 */
template<typename TPage, typename TKey>
class ARCCache {
    size_t sz_; ///< maximum number of cache pages
    size_t cache_hits_ = 0;
    size_t cache_accesses_ = 0;

    std::unordered_map<TKey, TPage> cache_; ///< hash table with cached pages
    std::list<TKey> B1, B2, T1, T2;         ///< lists for ARC algorithm
    size_t p = 0;                           ///< Preffered size of T1, being changed during runtime

    enum ARCList {  ///< ARC list names enum
        LNone = 0,
        LB1,
        LB2,
        LT1,
        LT2
    };

    using ListIt = typename std::list<TKey>::iterator;
    struct PageRefs {
        ListIt it;      ///< Iterator to element in list
        ARCList list;   ///< Specifiyng which list is iterator pointing to
        PageRefs(): it(), list(LNone) {}
        PageRefs(ListIt iter, ARCList l ): it(iter), list(l) {}
    };
    std::unordered_map<TKey, PageRefs> htable_; ///< Hash table with iterators to keys in B1, B2, T1 or T2 lists

    // subroutine used in ARC caching algorithm
    void replace(TKey key);

    void delete_LRU(std::list<TKey>& list, bool remove_from_cache);

    void move_to_MRU(std::list<TKey>& dst, std::list<TKey>& src, ListIt elem) {
        dst.splice(dst.begin(), src, elem);
    }
public:
    ARCCache(size_t size): sz_(size), T1(), T2(), B1(), B2(), cache_(), htable_() {}

    size_t get_hits()     const { return cache_hits_;     }
    size_t get_requests() const { return cache_accesses_; }

    template <typename F>
    TPage lookup_update(TKey key, F slow_get_page);

};

template<typename TPage, typename TKey>
void ARCCache<TPage, TKey>::delete_LRU(std::list<TKey>& list, bool remove_from_cache) {
    TKey key_to_delete = list.back();
    list.pop_back();
    htable_.erase(key_to_delete);
    if (remove_from_cache) {
        cache_.erase(key_to_delete);
    }
    // std::cout << "Deleted LRU " << key_to_delete << "\n";
}

template<typename TPage, typename TKey>
void ARCCache<TPage, TKey>::replace(TKey key) {
    bool key_in_B2 = (htable_.contains(key)) ? htable_[key].list == LB2 : false;
    if (!T1.empty() && (T1.size() > p || T1.size() == p && key_in_B2)) {
        TKey key_to_delete = T1.back();
        move_to_MRU(B1, T1, std::prev(T1.end()));
        cache_.erase(key_to_delete);
        htable_[key_to_delete].list = LB1;
    } else {
        // maybe redundant
        if (!T2.empty()) {
            TKey key_to_delete = T2.back();
            move_to_MRU(B2, T2, std::prev(T2.end()));
            cache_.erase(key_to_delete);
            htable_[key_to_delete].list = LB2;
        }
    }
}

template<typename TPage, typename TKey>
template<typename F>
TPage ARCCache<TPage, TKey>::lookup_update(TKey key, F slow_get_page) {
    cache_accesses_++;

    auto page_it = htable_.find(key);
    // ----------------- Key in ARC or DBL ---------------------------
    if (page_it != htable_.end() ) {
        PageRefs& page = page_it->second;
        switch (page.list) {
            // ----------------- Key in ARC (hit, page is stored in cache_) ---------------------------
            // Moving page from T1/T2 to MRU in T2
            case LT1:
                move_to_MRU(T2, T1, page.it);
                page.list = LT2;
                cache_hits_++;
                break;
            case LT2:
                move_to_MRU(T2, T2, page.it);
                cache_hits_++;
                break;
            // ----------------- Key in DBL (miss, page was stored not long ago) ----------------------
            case LB1:
            {
                // adaptation
                size_t delta = B1.size() >= B2.size() ? 1 : B2.size() / B1.size();
                p = std::min(sz_, p + delta);
                // replace
                replace(key);
                // moving key from B1 to MRU in T2
                move_to_MRU(T2, B1, page.it);
                page.list = LT2;
                // fetching
                cache_[key] = slow_get_page(key);
                break;
            }
            case LB2:
            {
                // adaptation
                size_t delta = B2.size() >= B1.size() ? 1 : B1.size() / B2.size();
                p = std::max((size_t)0, p - delta);
                // replace
                replace(key);
                // moving key from B2 to MRU in T2
                move_to_MRU(T2, B2, page.it);
                page.list = LT2;
                // fetching
                cache_[key] = slow_get_page(key);
            }
                break;
            case LNone:
            default:
                std::cout << "Found page without list\n" << key;
                exit(2);
                break;
        }
    } else {
    // ----------------- Key not found in B1, B2, T1 or T2 - miss ---------------------------
        size_t full_size = T1.size() + T2.size() + B1.size() + B2.size();
        // case A
        if (T1.size() + B1.size() == sz_) {
            if (T1.size() < sz_) {
                delete_LRU(B1, false);
                replace(key);
            } else {
                delete_LRU(T1, true);
            }
        // case B
        } else if (full_size >= sz_) {
            if (full_size == 2*sz_) {
                delete_LRU(B2, false);
            }
            replace(key);
        }

        T1.push_front(key);
        cache_[key] = slow_get_page(key);
        htable_[key] = PageRefs{T1.begin(), LT1};
    }
    return cache_[key];
}

template <typename T, typename KeyT = int> struct LRUCache {
  size_t sz_;
  std::list<std::pair<KeyT, T>> cache_;
  size_t hits_;

  using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
  std::unordered_map<KeyT, ListIt> hash_;

  LRUCache(size_t sz) : sz_(sz) {}

  bool full() const { return (cache_.size() == sz_); }

  template <typename F> bool lookup_update(KeyT key, F slow_get_page) {
    auto hit = hash_.find(key);
    if (hit == hash_.end()) {
      if (full()) {
        hash_.erase(cache_.back().first);
        cache_.pop_back();
      }
      cache_.emplace_front(key, slow_get_page(key));
      hash_.emplace(key, cache_.begin());
      return false;
    }

    auto eltit = hit->second;
    cache_.splice(cache_.begin(), cache_, eltit);
    hits_++;
    return true;
  }
};

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
