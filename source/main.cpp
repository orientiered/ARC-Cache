#include <iostream>
#include <stdlib.h>

#include "arc.hpp"


const int BAD_INPUT_EXIT = 2;


int get_page_plug(int key) {return key*2;}

int main() {
    int cache_size = 0, elements_count = 0;
    std::cin >> cache_size >> elements_count;

    if (cache_size <= 0 || elements_count < 0) {
        std::cout << "Invalid cache size or number of elements\n";
        return BAD_INPUT_EXIT;
    }

    IdealCache<int> ideal((size_t)cache_size);
    ARCCache<int, int> arc((size_t)cache_size);
    LRUCache<int, int> lru((size_t)cache_size);

    for (int elem_idx = 0; elem_idx < elements_count; elem_idx++) {
        int page_idx = -1;
        std::cin >> page_idx;
        arc.lookup_update(page_idx, get_page_plug);
        lru.lookup_update(page_idx, get_page_plug);
        ideal.lookup_update(page_idx);
    }

    std::cout << "ARC hits: " << arc.get_hits() << " out of " << elements_count << " requests \n";
    std::cout << "LRU hits: " << lru.hits_ << " out of " << elements_count << " requests \n";
    std::cout << "Ideal hits: " << ideal.get_hits() << " out of " << elements_count << " requests \n";

    return 0;
}
