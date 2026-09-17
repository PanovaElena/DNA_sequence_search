#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__KOKKOS__))

void naive_scalar(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len, KOKKOS_LAMBDA(int i,
        const MyViewStr data, const MyViewFreq freq, size_t len, size_t size) {
        uint32_t res=0;
        for(int j=0;j<size-len+1;++j){
            res += compare_strings_scalar(data, i, j, len);
        }
        freq[i]=res;
    });
}

#endif
