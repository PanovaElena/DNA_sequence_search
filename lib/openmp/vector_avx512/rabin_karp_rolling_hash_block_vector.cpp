#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__X86__) && defined(__OPENMP__))

void rabin_karp_rolling_hash_block_vector(std::vector<uint32_t>& freq_,
    const std::string& input_file, const int len) {
    std::vector<uint8_t> datav;
    std::vector<uint32_t> freqv;
    size_t size = 0;
    prepare_data(input_file, datav, freqv, size);
    
    uint8_t* data = datav.data();
	uint32_t* freq = freqv.data();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // ---------- run kernel ----------
    constexpr size_t block_lenint64 = 8;
    const size_t block_cycles = (size - len + 1) / block_lenint64;
    const size_t block_leftover = (size - len + 1) - block_cycles*block_lenint64;
    
    // block loop
#pragma omp parallel for
    for(int b = 0; b < block_cycles; ++b) {
        
        int i = b * block_lenint64;
        
        const uint64_t p = 2;
        uint64_t powmod = 1;
        __m512i vone = _mm512_set1_epi64(1);
        __m512i vzero = _mm512_xor_epi64(vone, vone);
        __m512i vpowmod = vone;  // =1
        __m512i vp = _mm512_add_epi64(vone, vone);  // =2
        
        __m512i vhash_pattern = _mm512_xor_epi64(vp, vp);
        uint64_t hash_text = 0;
        
        for (int j = 0; j < len; ++j) {
            vhash_pattern = _mm512_add_epi64(_mm512_mullo_epi64(vhash_pattern, vp),
                _mm512_cvtepu8_epi64(_mm_loadu_epi8(&data[i + j])));
            hash_text = hash_text * p + data[j];
            
            vpowmod = _mm512_mullo_epi64(vpowmod, vp);
            powmod = powmod * p;
        }
        
        uint32_t res[block_lenint64] = {0};
        
        for (int j = 0; j < size - len + 1; ++j) {
            __m512i vhash_text = _mm512_set1_epi64(hash_text);
            auto vmask = _mm512_cmpeq_epi64_mask(vhash_pattern, vhash_text);
            
            uint32_t mask = _cvtmask8_u32(vmask);
            
            while (mask) {
                int t = _tzcnt_u32(mask);
                
                //res[t] += compare_strings_vector_16(data, i + t, j, len);
                int is_eq = 1;
                for(int k = 0; k < len && is_eq; ++k) {
                    if (data[i + t + k] != data[j + k]) {
                        is_eq = 0;
                    }
                }
                res[t] += is_eq;
                
                mask = _blsr_u32(mask);
            }
            
            hash_text = (hash_text * p - data[j] * powmod + data[j + len]);
        }
        
        #pragma unroll
        for (int t = 0; t < block_lenint64; t++)
            freq[i+t] = res[t];
    }

// remainder  
#pragma omp parallel for
    for(int i = size - len + 1 - block_leftover; i < size - len + 1; ++i) {
        int res = 0;
        uint64_t hash_pattern = 0;
        uint64_t hash_text = 0;
        uint64_t p = 2;
        uint64_t powmod = 1;
        for (int j = 0; j < len; ++j) {
            hash_pattern = hash_pattern * p + data[i + j];
            hash_text = hash_text * p + data[j];
            powmod = powmod * p;
        }
        for (int j = 0; j < size - len + 1; ++j) {
            if (hash_text == hash_pattern) {
                int is_eq = 1;
                for(int k = 0; k < len && is_eq; ++k) {
                    if (data[i + k] != data[j + k]) {
                        is_eq = 0;
                    }
                }
                res += is_eq;
            }
            hash_text = (hash_text * p - data[j] * powmod + data[j + len]);
        }
        freq[i] = res;
    }
    // ---------- end kernel ----------
    
    auto stop = std::chrono::high_resolution_clock::now();
    
    freq_.resize(size);
    for (size_t i = 0; i < size; ++i) freq_[i] = freq[i];
    
    print_result(size, len, (std::chrono::duration<double>(stop - start)).count()); 
}

#endif
