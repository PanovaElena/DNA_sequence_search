#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__CUDA__))

void rabin_karp_SWAR_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    std::cout << "Vector SWAR is not avaliable for Cuda" << std::endl;
}

#endif
