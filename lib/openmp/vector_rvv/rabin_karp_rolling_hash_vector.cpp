#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__RISCV__) && defined(__OPENMP__))

#define LMUL 1

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define ADD_POSTFIX(line, postfix) CONCAT(line, CONCAT(m, postfix))
#define ADD_LMUL(line) ADD_POSTFIX(line, LMUL)

void rabin_karp_rolling_hash_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        int res = 0;
        uint64_t hash_pattern = 0;
        uint64_t hash_text = 0;
        uint64_t p = 2;
        uint64_t powmod = 1;
        for (int j = 0; j < len; ++j) {
            hash_pattern = hash_pattern * p + data[i + j];
            hash_text = hash_text * p + data[j];
            powmod = powmod * p;
        }
        for (int j = 0; j < size - len + 1; ++j) {
            if (hash_text == hash_pattern) {
                res += ADD_LMUL(compare_strings_vector8)(data, i, j, len);
            }
            hash_text = (hash_text * p - data[j] * powmod + data[j + len]);
        }
        freq[i] = res;
    });
}

#endif
