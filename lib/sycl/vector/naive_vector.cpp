#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__SYCL__) && defined(__ESIMD__))

void naive_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len){
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        uint32_t res=0;
        for(int j=0;j<size-len+1;++j){
            res+=compare_strings_vector(data, i, j, len);
        }
        freq[i]=res;
    });
}

#endif
