#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__CUDA__))

__global__ void search_kernel_rabin_karp_SWAR2_vector(uint8_t* data, uint32_t* freq, int len, int size) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= size - len + 1) return;
    
    const int lenbytes_int32 = 4;
    uint32_t res = 0;
    uint32_t vpattern_elem1234 = cuda_load_int32(data, i);
    
    for (int j = 0; j < size - len + 1; j += lenbytes_int32) {
        uint32_t v0text_elem1234 = cuda_load_int32(data, j+0);
        uint32_t v1text_elem1234 = cuda_load_int32(data, j+1);
        uint32_t v2text_elem1234 = cuda_load_int32(data, j+2);
        uint32_t v3text_elem1234 = cuda_load_int32(data, j+3);
        
        auto bitmaskh0 = vpattern_elem1234 == v0text_elem1234;
        auto bitmaskh1 = vpattern_elem1234 == v1text_elem1234;
        auto bitmaskh2 = vpattern_elem1234 == v2text_elem1234;
        auto bitmaskh3 = vpattern_elem1234 == v3text_elem1234;
        
        if (bitmaskh0) {
            res+=compare_strings_vector(data, i, j+0, len);
        }
        if (bitmaskh1) {
            res+=compare_strings_vector(data, i, j+1, len);
        }
        if (bitmaskh2) {
            res+=compare_strings_vector(data, i, j+2, len);
        }
        if (bitmaskh3) {
            res+=compare_strings_vector(data, i, j+3, len);
        }
    }
    freq[i] = res;
}

void rabin_karp_SWAR2_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len, search_kernel_rabin_karp_SWAR2_vector);
}

#endif
