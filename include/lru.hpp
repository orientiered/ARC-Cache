#pragma once

#include <list>
#include <unordered_map>

/// @file Header-only imlpementation of LRU cache algorithm

template <typename T, typename KeyT = int>
class LRUCache {
public:
  LRUCache(std::size_t sz) : sz_(sz), cache_(), hash_() {}

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

    std::size_t get_hits() const { return hits_; }
private:
  std::size_t sz_;
  std::list<std::pair<KeyT, T>> cache_;
  std::size_t hits_ = 0;

  using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
  std::unordered_map<KeyT, ListIt> hash_;

  bool full() const { return (cache_.size() == sz_); }
};
