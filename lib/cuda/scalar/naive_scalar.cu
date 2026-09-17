#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__CUDA__))

__global__ void search_kernel_naive_scalar(uint8_t* data, uint32_t* freq, int len, int size) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= size - len + 1) return;
    
    int res = 0;
    for (int j = 0; j < size - len + 1; ++j) {
        res += compare_strings_scalar(data, i, j, len);
    }
    freq[i] = res;
}

void naive_scalar(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len, search_kernel_naive_scalar);
}

#endif
