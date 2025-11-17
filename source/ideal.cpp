#include <iostream>

#include "ideal.hpp"
#include "input.hpp"

int main() {

    unsigned cache_size = 0, elements_count = 0;

    if (read_input_header(cache_size, elements_count)) {
        return EXIT_FAILURE;
    }

    IdealCache<int> ideal((size_t)cache_size);
    for (unsigned elem_idx = 0; elem_idx < elements_count; elem_idx++) {
        int page_idx = -1;
        std::cin >> page_idx;
        check_cin("Invalid page idx\n");
        ideal.lookup_update(page_idx);
    }

    std::cout << ideal.get_hits() << "\n";

    return 0;
}
