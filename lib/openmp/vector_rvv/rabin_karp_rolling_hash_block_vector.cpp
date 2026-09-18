#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__RISCV__) && defined(__OPENMP__))

#define LMUL 4
#define MASKL 16

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define ADD_POSTFIXM(line, postfix) CONCAT(line, CONCAT(m, postfix))
#define ADD_POSTFIXB(line, postfix) CONCAT(line, CONCAT(_b, postfix))
#define ADD_LMUL(line) ADD_POSTFIXM(line, LMUL)
#define ADD_MASKL(line) ADD_POSTFIXB(line, MASKL)

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
    size_t vl = ADD_LMUL(__riscv_vsetvl_e64)(size - len + 1);
    
#pragma omp parallel for
    for (int i = 0; i < size - len + 1; i += vl) {
        vl = ADD_LMUL(__riscv_vsetvl_e64)(size - len + 1 - i);
        
        const uint64_t p = 2;
        uint64_t powmod = 1;
        
        auto vhash_pattern = ADD_LMUL(__riscv_vmv_v_x_u64)(0, vl);
        uint64_t hash_text = 0;
        
        for (int j = 0; j < len; ++j) {
            uint64_t tmp[512] = {0};
            #pragma unroll
            for (int t = 0; t < vl; t++) tmp[t] = data[i + j + t];
            
            vhash_pattern = ADD_LMUL(__riscv_vadd_vv_u64)(
                ADD_LMUL(__riscv_vmul_vx_u64)(vhash_pattern, p, vl),
                ADD_LMUL(__riscv_vle64_v_u64)(tmp, vl), vl);
            hash_text = hash_text * p + data[j]; 
            powmod = powmod * p;
        }
        
        uint32_t res[512] = {0};
        
        for (int j = 0; j < size - len + 1; ++j) {
            auto vmask = ADD_MASKL(ADD_LMUL(__riscv_vmseq_vx_u64))(vhash_pattern, hash_text, vl);
            int a = ADD_MASKL(__riscv_vcpop_m)(vmask, vl);
            
            int t = ADD_MASKL(__riscv_vfirst_m)(vmask, vl);
            while (t >= 0) {
                res[t] += ADD_LMUL(compare_strings_vector8)(data, i + t, j, len);
                
                auto mask_trailing = ADD_MASKL(__riscv_vmsof_m)(vmask, vl);
                vmask = ADD_MASKL(__riscv_vmandn_mm)(vmask, mask_trailing, vl);
                t = ADD_MASKL(__riscv_vfirst_m)(vmask, vl);
            }
            
            hash_text = (hash_text * p - data[j] * powmod + data[j + len]);
        }
        
        #pragma unroll
        for (int t = 0; t < vl; t++)
            freq[i + t] = res[t];
    }
    
    auto stop = std::chrono::high_resolution_clock::now();
    
    freq_.resize(size);
    for (size_t i = 0; i < size; ++i) freq_[i] = freq[i];
    
    print_result(size, len, (std::chrono::duration<double>(stop - start)).count());   
}

#endif
