#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__X86__) && defined(__OPENMP__))

void naive_block_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len){
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        uint32_t res = 0;
        
        constexpr size_t block_len = 64;
        const size_t block_cycles = (size - len + 1) / block_len;
        const size_t block_leftover = (size - len + 1) - block_cycles*block_len;
        
        for(int b = 0; b < block_cycles; ++b) {
            int j = b * block_len;
            __mmask64 mask_eq = 0xffffffffffffffff;
            
            for (int k = 0; k < len; k++) {
                auto vpattern_symb = _mm512_set1_epi8(data[i + k]);
                auto vtext = _mm512_loadu_epi8(&data[j + k]);
                mask_eq = mask_eq & _mm512_cmpeq_epu8_mask(vpattern_symb, vtext);
                if (mask_eq == 0x0000000000000000) break;
            }
            
            res += _mm_popcnt_u64(_cvtmask64_u64(mask_eq));
            
        }
        for(int j = block_cycles*block_len; j < size-len+1; ++j) {
            int is_eq = 1;
            for(int k = 0; k < len && is_eq; ++k) {
                if (data[i + k] != data[j + k]) {
                    is_eq = 0;
                }
            }
            res += is_eq;
        }
        
        freq[i]=res;
    });
}

#endif
