#include <iostream>

#include "ideal.hpp"

const int BAD_INPUT_EXIT = 2;

int main(int argc, const char *argv[]) {

    int cache_size = 0, elements_count = 0;
    std::cin >> cache_size >> elements_count;

    if (cache_size <= 0 || elements_count < 0) {
        std::cout << "Invalid cache size or number of elements\n";
        return BAD_INPUT_EXIT;
    }

    IdealCache<int> ideal((size_t)cache_size);
    for (int elem_idx = 0; elem_idx < elements_count; elem_idx++) {
        int page_idx = -1;
        std::cin >> page_idx;
        ideal.lookup_update(page_idx);
    }

    std::cout << ideal.get_hits() << "\n";

    return 0;
}
