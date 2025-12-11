#include <iostream>
#include <stdexcept>

#include "input.hpp"

namespace Caches {

void check_cin(const std::string err_msg) {
    if (std::cin.fail()) {
        std::cerr << err_msg;
        throw cin_error("Error when reading from console");
    }
    return;
}

int read_input_header(unsigned &cache_size, unsigned &elements_count) {
    long long cache_size_ = 0, elements_count_ = 0;
    std::cin >> cache_size_;
    check_cin("Failed to read cache size\n");

    if (cache_size_ <= 0) {
        std::cerr << "Invalid cache size: only positive values are accepted\n";
        return EXIT_FAILURE;
    }

    std::cin >> elements_count_;
    check_cin("Failed to read number of elements\n");

    if (elements_count_ <= 0) {
        std::cerr << "Invalid number of elements: only positive values are accepted\n";
        return EXIT_FAILURE;
    }

    cache_size = (unsigned) cache_size_;
    elements_count = (unsigned) elements_count_;

    return 0;
}

} // namespace Caches
