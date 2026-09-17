#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__KOKKOS__))

void rabin_karp_SWAR_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    std::cout << "ERROR: vector SWAR is not avaliable for Kokkos" << std::endl;
}

#endif
