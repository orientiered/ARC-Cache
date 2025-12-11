#include <iostream>
#include <stdexcept>

#include "ideal.hpp"
#include "input.hpp"

int main() {

    unsigned cache_size = 0, elements_count = 0;

    try {
        if (Caches::read_input_header(cache_size, elements_count)) {
            return EXIT_FAILURE;
        }

        Caches::IdealCache<int> ideal((size_t)cache_size);

        for (unsigned elem_idx = 0; elem_idx < elements_count; elem_idx++) {
            int page_idx = -1;
            std::cin >> page_idx;
            Caches::check_cin("Invalid page idx\n");
            ideal.lookup_update(page_idx);
        }

        std::cout << ideal.get_hits() << "\n";

    } catch(const Caches::cin_error& err) {
        std::cerr << err.what() << "\n";
        return EXIT_FAILURE;
    }


    return 0;
}
