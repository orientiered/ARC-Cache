#include <fstream>
#include <random>
#include <filesystem>
#include <vector>
#include <iostream>
#include <cstring>

struct TestCase {
    size_t count;
    size_t cache_size;
    int low, high;
};

const int MT_SEED = 275;

using KeyType = int;

struct TestData {
    size_t count;
    size_t cache_size;
    std::vector<KeyType> data;
};

TestData create_test(const TestCase& test) {
    std::mt19937 mt_engine(MT_SEED);
    std::uniform_int_distribution<> gen(test.low, test.high);

    std::vector<KeyType> result(test.count);

    for (size_t idx = 0; idx < test.count; idx++) {
        result[idx] = gen(mt_engine);
    }

    return TestData{test.count, test.cache_size, result};
}

std::string write_test(const std::string dir, size_t idx, const TestData& data) {
    using path_t = std::filesystem::path;

    std::string file_name = std::to_string(idx) + ".dat";

    path_t path = path_t{dir} / file_name;

    std::fstream test_file;
    test_file.open(path, std::ios::in | std::ios::out | std::ios::trunc);
    if (!test_file.is_open()) {
        std::cerr << "Failed to open file " << path << "\n";
        std::cerr << "Error details: " << strerror(errno) << "\n";
        return "ERROR";
    }

    test_file << data.cache_size << " " << data.count << "\n";

    for (const KeyType& key: data.data) {
        test_file << key << " ";
    }

    test_file << "\n";
    test_file.close();

    return path.string();
}

int generate_tests(const std::string dir, const std::vector<TestCase>& Tests) {
    if (!std::filesystem::is_directory(dir)) {
        std::cerr << dir << " is not directory\n";
        return -1;
    }

    std::cout << "Found " << Tests.size() << " test cases\n";
    for (size_t idx = 0; idx < Tests.size(); idx++) {
        TestData data = create_test(Tests[idx]);

        std::string path = write_test(dir, idx, data);
        std::cout << "Writed test " << idx << " to " << path << "\n";
    }

    return 0;
}

const std::vector<TestCase> Tests = {
    TestCase{.count =   1000, .cache_size =   15, .low = 0, .high =    30},
    TestCase{.count =   1000, .cache_size =   30, .low = 0, .high =  2000},
    TestCase{.count = 300000, .cache_size =   50, .low = 0, .high =   100},
    TestCase{.count = 300000, .cache_size = 1000, .low = 0, .high = 25000},
    TestCase{.count = 300000, .cache_size = 10000, .low = 0, .high = 25000}
};

int main() {
    generate_tests("tests", Tests);
    return 0;
}
