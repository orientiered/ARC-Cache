#include <string>

/// @brief Safely read cache size and number of elements from stdin
int read_input_header(unsigned &cache_size, unsigned &elements_count);

/// @brief Check if std::cin is good and terminate program with err_msg on failure
void check_cin(const std::string err_msg);
