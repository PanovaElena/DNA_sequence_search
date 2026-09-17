#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__SYCL__) && defined(__ESIMD__))

void rabin_karp_SWAR2_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        constexpr size_t simd_lenbytes = simd_lenint32 * 4;
		
        uint32_t res = 0;       
        uint32_t pattern_elem1234 = static_cast<uint32_t>(data[i]) |
            (static_cast<uint32_t>(data[i + 1]) << 8) |
            (static_cast<uint32_t>(data[i + 2]) << 16) |
            (static_cast<uint32_t>(data[i + 3]) << 24);
        simd_type vpattern_elem1234(pattern_elem1234);
        
        for (int j = 0; j < size - len + 1; j += simd_lenbytes) {
            simd_type v0text_elem1234, v1text_elem1234, v2text_elem1234, v3text_elem1234;
            
            load_simd(data, j+0, v0text_elem1234);
            load_simd(data, j+1, v1text_elem1234);
            load_simd(data, j+2, v2text_elem1234);
            load_simd(data, j+3, v3text_elem1234);
            
            auto bitmaskh0 = vpattern_elem1234 == v0text_elem1234;
            auto bitmaskh1 = vpattern_elem1234 == v1text_elem1234;
            auto bitmaskh2 = vpattern_elem1234 == v2text_elem1234;
            auto bitmaskh3 = vpattern_elem1234 == v3text_elem1234;
            
            for(int t = 0; t < simd_lenint32; t++) {
                if (bitmaskh0[t]) {
                    res+=compare_strings_vector(data, i, j+0+t*4, len);
                }
                if (bitmaskh1[t]) {
                    res+=compare_strings_vector(data, i, j+1+t*4, len);
                }
                if (bitmaskh2[t]) {
                    res+=compare_strings_vector(data, i, j+2+t*4, len);
                }
                if (bitmaskh3[t]) {
                    res+=compare_strings_vector(data, i, j+3+t*4, len);
                }
            }
        }
        freq[i] = res;
    });
}

#endif
