#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__RISCV__) && defined(__OPENMP__))

#define LMUL 4
#define MASKL 2

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define ADD_POSTFIXM(line, postfix) CONCAT(line, CONCAT(m, postfix))
#define ADD_POSTFIXB(line, postfix) CONCAT(line, CONCAT(_b, postfix))
#define ADD_LMUL(line) ADD_POSTFIXM(line, LMUL)
#define ADD_MASKL(line) ADD_POSTFIXB(line, MASKL)

void naive_block_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len){
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        uint32_t res = 0;
        
        size_t vl = ADD_LMUL(__riscv_vsetvl_e8)(size - len + 1);
        
        auto mask_true = ADD_MASKL(__riscv_vmset_m)(vl);
        auto mask_false = ADD_MASKL(__riscv_vmnot_m)(mask_true, vl);
        
        for(int j = 0; j < size - len + 1; j += vl) {
            vl = ADD_LMUL(__riscv_vsetvl_e8)(size - len + 1 - j);
            auto mask_eq = mask_true;
            
            int popc_res = 0;
            
            for (int k = 0; k < len; k++) {
                auto vtext = ADD_LMUL(__riscv_vle8_v_u8)(&data[j + k], vl);
                auto cur_mask = ADD_MASKL(ADD_LMUL(__riscv_vmseq_vx_u8))(vtext, data[i + k], vl);
                mask_eq = ADD_MASKL(__riscv_vmand_mm)(mask_eq, cur_mask, vl);
                
                popc_res = ADD_MASKL(__riscv_vcpop_m)(mask_eq, vl);
                if (popc_res == 0) break;
            }
            
            res += popc_res;
            
        }
        
        freq[i]=res;
    });
}

#endif
