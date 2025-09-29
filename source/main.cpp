#include <iostream>
#include <string.h>

#include "arc.hpp"
#include "lru.hpp"
#include "ideal.hpp"


int get_page_plug(int key);
int read_input_header(unsigned &cache_size, unsigned &elements_count);

int get_page_plug(int key) {return key*2;}

int read_input_header(unsigned &cache_size, unsigned &elements_count) {
    long long cache_size_ = 0, elements_count_ = 0;
    std::cin >> cache_size_;
    if (!std::cin.good()) {
        std::cerr << "Failed to read cache size\n";
        return EXIT_FAILURE;
    }

    if (cache_size_ <= 0) {
        std::cerr << "Invalid cache size: only positive values are accepted\n";
        return EXIT_FAILURE;
    }

    std::cin >> elements_count_;
    if (!std::cin.good()) {
        std::cerr << "Failed to read number of elements\n";
        return EXIT_FAILURE;
    }

    if (elements_count_ <= 0) {
        std::cerr << "Invalid number of elements: only positive values are accepted\n";
        return EXIT_FAILURE;
    }

    cache_size = (unsigned) cache_size_;
    elements_count = (unsigned) elements_count_;

    return 0;
}

int main(int argc, const char *argv[]) {

    bool verbose = false;
    if (argc > 1 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--verbose") == 0)) {
        verbose = true;
    }

    unsigned cache_size = 0, elements_count = 0;
    if (read_input_header(cache_size, elements_count)) {
        return EXIT_FAILURE;
    }

    using TKey = int;

    IdealCache<TKey> ideal((size_t)cache_size);
    ARCCache<int, TKey> arc((size_t)cache_size);
    LRUCache<int, TKey> lru((size_t)cache_size);

    for (unsigned elem_idx = 0; elem_idx < elements_count; elem_idx++) {
        TKey page_idx{};
        std::cin >> page_idx;
        if (!std::cin.good()) {
            std::cerr << "Failed to read page index\n";
            return EXIT_FAILURE;
        }

        arc.lookup_update(page_idx, get_page_plug);
        if (verbose) {
            lru.lookup_update(page_idx, get_page_plug);
            ideal.lookup_update(page_idx);
        }
    }

    if (verbose) {
        std::cout << "ARC hits:   " << arc.get_hits()   << " out of " << elements_count << " requests \n";
        std::cout << "LRU hits:   " << lru.get_hits()   << " out of " << elements_count << " requests \n";
        std::cout << "Ideal hits: " << ideal.get_hits() << " out of " << elements_count << " requests \n";
    } else {
        std::cout << arc.get_hits() << "\n";
    }

    return 0;
}
