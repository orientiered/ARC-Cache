#include <iostream>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <list>

const int BAD_INPUT_EXIT = 2;

template<typename TPage, typename TKey>
class ARCCache {
    // algo source: https://www2.cs.uh.edu/~paris/7360/PAPERS03/arcfast.pdf
    size_t sz_;

    std::unordered_map<TKey, TPage> cache_;
    std::list<TKey> B1, B2, T1, T2;
    size_t p = 0;

    using ListIt = typename std::list<TKey>::iterator;
    enum ARCList {
        LNone = 0,
        LB1,
        LB2,
        LT1,
        LT2
    };

    struct PageRefs {
        PageRefs(): it(), list(LNone) {}
        PageRefs(ListIt iter, ARCList l ): it(iter), list(l) {}
        ListIt it;
        ARCList list;
    };
    std::unordered_map<TKey, PageRefs> htable_;

    void replace(TKey key) {
        bool key_in_B2 = (htable_.contains(key)) ? htable_[key].list == LB2 : false;
        if (!T1.empty() && (T1.size() > p || T1.size() == p && key_in_B2)) {
            TKey key_to_delete = T1.back();
            T1.pop_back();
            cache_.erase(key_to_delete);
            B1.push_front(key_to_delete);
            htable_[key_to_delete] = {B1.begin(), LB1};
        } else {
            TKey key_to_delete = T2.back();
            T2.pop_back();
            cache_.erase(key_to_delete);
            B2.push_front(key_to_delete);
            htable_[key_to_delete] = {B2.begin(), LB2};
        }
    }

    void delete_LRU(std::list<TKey>& list, bool remove_from_cache) {
        TKey key_to_delete = list.back();
        list.pop_back();
        htable_.erase(key_to_delete);
        if (remove_from_cache) {
            cache_.erase(key_to_delete);
        }
        std::cout << "Deleted LRU " << key_to_delete << "\n";
    }
public:
    ARCCache(size_t size): sz_(size), T1(), T2(), B1(), B2(), cache_(), htable_() {}

    template <typename F>
    bool lookup_update(TKey key, F slow_get_page) {
        auto page_it = htable_.find(key);
        // hit
        if (page_it != htable_.end() ) {
            PageRefs& page = page_it->second;
            if (page.list == LT1) {
                // moving page from T1 to MRU in T2
                T1.erase(page.it);
                T2.push_front(key);
                page.list = LT2;
                page.it = T2.begin();
                return true;
            } else if (page.list == LT2) {
                // moving page to MRU in T2
                //TODO: replace with splice
                T2.erase(page.it);
                T2.push_front(key);
                page.it = T2.begin();
                return true;
            } else if (page.list == LB1) {
                // adaptation
                size_t delta = B1.size() >= B2.size() ? 1 : B2.size() / B1.size();
                p = std::min(sz_, p + delta);
                // replace
                replace(key);
                // moving key from B1 to MRU in T2
                B1.erase(page.it);
                T2.push_front(key);
                page.list = LT2;
                page.it = T2.begin();
                // fetching
                cache_[key] = slow_get_page(key);
                return false;
            } else if (page.list == LB2) {
                // adaptation
                size_t delta = B2.size() >= B1.size() ? 1 : B1.size() / B2.size();
                p = std::max((size_t)0, p - delta);
                // replace
                replace(key);
                // moving key from B2 to MRU in T2
                B2.erase(page.it);
                T2.push_front(key);
                page.list = LT2;
                page.it = T2.begin();
                // fetching
                cache_[key] = slow_get_page(key);
                return false;
            } else {
                std::cout << "Found page without list\n" << key;
                exit(2);
            }
        } else {
        // miss
            // case A
            if (T1.size() + B1.size() == sz_) {
                if (T1.size() < sz_) {
                    delete_LRU(B1, false);
                    replace(key);
                } else {
                    delete_LRU(T1, true);
                }
            // case B
            } else if (T1.size() + T2.size() + B1.size() + B2.size() >= sz_) {
                if (T1.size() + T2.size() + B1.size() + B2.size() == 2*sz_) {
                    delete_LRU(B2, false);
                }
                replace(key);
            }

            T1.push_front(key);
            cache_[key] = slow_get_page(key);
            htable_[key] = PageRefs{T1.begin(), LT1};
        }
        return false;
    }
};

template<typename TKey>
class IdealCache {
public:
    IdealCache(size_t cache_size): size_(cache_size), requests() {
        requests.reserve(10);
    }

    void getPage(TKey key) {
        requests.push_back(key);
    }

    size_t getHits();
private:
    std::vector<TKey> requests;
    void dropElem(std::unordered_map<TKey, int>& cache, size_t cur_idx);
    size_t size_;
};


int get_page_plug(int key) {return key*2;}

int main() {
    std::cout << "Hello world\n";

    int cache_size = 0, elements_count = 0;
    std::cin >> cache_size >> elements_count;

    if (cache_size <= 0 || elements_count < 0) {
        std::cout << "Invalid cache size or number of elements\n";
        return BAD_INPUT_EXIT;
    }

    // IdealCache<int> ideal((size_t)cache_size);
    ARCCache<int, int> arc((size_t)cache_size);

    int hits = 0;
    for (int elem_idx = 0; elem_idx < elements_count; elem_idx++) {
        int page_idx = -1;
        std::cin >> page_idx;
        hits += arc.lookup_update(page_idx, get_page_plug);
        // ideal.getPage(page_idx);
    }

    std::cout << "ARC hits: " << hits << " out of " << elements_count << "\n";

    return 0;
}
