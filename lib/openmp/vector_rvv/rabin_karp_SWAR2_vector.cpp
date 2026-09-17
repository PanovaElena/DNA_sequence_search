#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__RISCV__) && defined(__OPENMP__))

#define LMUL 2

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define ADD_POSTFIX(line, postfix) CONCAT(line, CONCAT(m, postfix))
#define ADD_LMUL(line) ADD_POSTFIX(line, LMUL)
#define TYPE_LMUL(type) CONCAT(ADD_POSTFIX(type, LMUL), _t)

void rabin_karp_SWAR2_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        const size_t elems_int32 = (size - len + 1) / 4 + 1;
        size_t vl = ADD_LMUL(__riscv_vsetvl_e32)(elems_int32);
        
        uint32_t res = 0;
        uint32_t pattern_elem1234 = static_cast<uint32_t>(data[i]) |
            (static_cast<uint32_t>(data[i + 1]) << 8) |
            (static_cast<uint32_t>(data[i + 2]) << 16) |
            (static_cast<uint32_t>(data[i + 3]) << 24);
        
        for (int j = 0; j < elems_int32; j += vl) {
            vl = ADD_LMUL(__riscv_vsetvl_e32)(elems_int32 - j);
            
            TYPE_LMUL(vuint32) v0text_elem1234, v1text_elem1234, v2text_elem1234, v3text_elem1234; 
            ADD_LMUL(load_simd32)(&data[j*4+0], v0text_elem1234, vl);
            ADD_LMUL(load_simd32)(&data[j*4+1], v1text_elem1234, vl);
            ADD_LMUL(load_simd32)(&data[j*4+2], v2text_elem1234, vl);
            ADD_LMUL(load_simd32)(&data[j*4+3], v3text_elem1234, vl);
            
            TYPE_LMUL(vuint32) v0eq_elem1234 = ADD_LMUL(__riscv_vxor_vx_u32)(v0text_elem1234, pattern_elem1234, vl);
            TYPE_LMUL(vuint32) v1eq_elem1234 = ADD_LMUL(__riscv_vxor_vx_u32)(v1text_elem1234, pattern_elem1234, vl);
            TYPE_LMUL(vuint32) v2eq_elem1234 = ADD_LMUL(__riscv_vxor_vx_u32)(v2text_elem1234, pattern_elem1234, vl);
            TYPE_LMUL(vuint32) v3eq_elem1234 = ADD_LMUL(__riscv_vxor_vx_u32)(v3text_elem1234, pattern_elem1234, vl);
            
            uint32_t tmp0[512], tmp1[512], tmp2[512], tmp3[512];
            ADD_LMUL(__riscv_vse32_v_u32)(tmp0, v0eq_elem1234, vl);
            ADD_LMUL(__riscv_vse32_v_u32)(tmp1, v1eq_elem1234, vl);
            ADD_LMUL(__riscv_vse32_v_u32)(tmp2, v2eq_elem1234, vl);
            ADD_LMUL(__riscv_vse32_v_u32)(tmp3, v3eq_elem1234, vl);
            
            for(int t = 0; t < vl; t++) {
                if (tmp0[t] == 0) {
                    res += ADD_LMUL(compare_strings_vector32)(data, i, j*4+0+t*4, len);
                }
                if (tmp1[t] == 0) {
                    res += ADD_LMUL(compare_strings_vector32)(data, i, j*4+1+t*4, len);
                }
                if (tmp2[t] == 0) {
                    res += ADD_LMUL(compare_strings_vector32)(data, i, j*4+2+t*4, len);
                }
                if (tmp3[t] == 0) {
                    res += ADD_LMUL(compare_strings_vector32)(data, i, j*4+3+t*4, len);
                }
            }
        }
        freq[i] = res;
    });
}

#endif
