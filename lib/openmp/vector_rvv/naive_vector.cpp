#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__RISCV__) && defined(__OPENMP__))

#define LMUL 1

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define ADD_POSTFIX(line, postfix) CONCAT(line, CONCAT(m, postfix))
#define ADD_LMUL(line) ADD_POSTFIX(line, LMUL)

void naive_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len){
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        uint32_t res=0;
        for(int j=0;j<size-len+1;++j){
            res += ADD_LMUL(compare_strings_vector8)(data, i, j, len);
        }
        freq[i]=res;
    });
}

#endif
