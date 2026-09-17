#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__KOKKOS__))

void rabin_karp_rolling_hash_scalar(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len, KOKKOS_LAMBDA(int i,
        const MyViewStr data, const MyViewFreq freq, size_t len, size_t size) {
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
                res += compare_strings_scalar(data, i, j, len);
            }
            hash_text = (hash_text * p - data[j] * powmod + data[j + len]);
        }
        freq[i] = res;
    });
}

#endif
