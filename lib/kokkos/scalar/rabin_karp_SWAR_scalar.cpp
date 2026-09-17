#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__KOKKOS__))

void rabin_karp_SWAR_scalar(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len, KOKKOS_LAMBDA(int i,
        const MyViewStr data, const MyViewFreq freq, size_t len, size_t size) {
        uint32_t res = 0;
        uint8_t pattern_elem1 = data[i];
        uint8_t pattern_elem2 = data[i+1];
        uint8_t pattern_elem3 = data[i+len-2];
        uint8_t pattern_elem4 = data[i+len-1];
        
        for (int j = 0; j < size - len + 1; ++j) {
            uint8_t text_elem1 = data[j];
            uint8_t text_elem2 = data[j+1];
            uint8_t text_elem3 = data[j+len-2];
            uint8_t text_elem4 = data[j+len-1];
            
            bool cmp = (pattern_elem1 == text_elem1) &&
                (pattern_elem2 == text_elem2) &&
                (pattern_elem3 == text_elem3) &&
                (pattern_elem4 == text_elem4);
            
            if (cmp) {
                res += compare_strings_scalar(data, i, j, len);
            }
        }
        freq[i] = res;
    });
}

#endif
