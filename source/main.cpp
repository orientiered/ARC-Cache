#include <iostream>
#include <string.h>

#include "arc.hpp"
#include "lru.hpp"
#include "ideal.hpp"
#include "input.hpp"

int get_page_plug(int key);

int get_page_plug(int key) {return key*2;}

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

    try {
        for (unsigned elem_idx = 0; elem_idx < elements_count; elem_idx++) {
            TKey page_idx{};
            std::cin >> page_idx;
            check_cin("Failed to read page index\n");

            arc.lookup_update(page_idx, get_page_plug);
            if (verbose) {
                lru.lookup_update(page_idx, get_page_plug);
                ideal.lookup_update(page_idx);
            }
        }
    } catch (const std::logic_error& err) {
        std::cerr << err.what() << "\n";
        return EXIT_FAILURE;
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
