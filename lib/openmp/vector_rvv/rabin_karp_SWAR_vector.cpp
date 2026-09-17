#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__RISCV__) && defined(__OPENMP__))

#define LMUL 4

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

#define ADD_POSTFIX(line, postfix) CONCAT(line, CONCAT(m, postfix))
#define ADD_LMUL(line) ADD_POSTFIX(line, LMUL)
#define TYPE_LMUL(type) CONCAT(ADD_POSTFIX(type, LMUL), _t)

void rabin_karp_SWAR_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        size_t vl = ADD_LMUL(__riscv_vsetvl_e8)(size - len + 1);
		
		uint32_t res = 0;
		uint8_t pattern_elem1 = data[i];
		uint8_t pattern_elem2 = data[i+1];
		uint8_t pattern_elem3 = data[i+len-2];
		uint8_t pattern_elem4 = data[i+len-1];
		
		for (int j = 0; j < size - len + 1; j += vl) {
            vl = ADD_LMUL(__riscv_vsetvl_e8)(size - len + 1 - j);
            
			TYPE_LMUL(vuint8) vtext_elem1 = ADD_LMUL(__riscv_vle8_v_u8)(&data[j], vl);
			TYPE_LMUL(vuint8) vtext_elem2 = ADD_LMUL(__riscv_vle8_v_u8)(&data[j+1], vl);
			TYPE_LMUL(vuint8) vtext_elem3 = ADD_LMUL(__riscv_vle8_v_u8)(&data[j+len-2], vl);
			TYPE_LMUL(vuint8) vtext_elem4 = ADD_LMUL(__riscv_vle8_v_u8)(&data[j+len-1], vl);
			
			TYPE_LMUL(vuint8) veq_elem1 = ADD_LMUL(__riscv_vxor_vx_u8)(vtext_elem1, pattern_elem1, vl);
			TYPE_LMUL(vuint8) veq_elem2 = ADD_LMUL(__riscv_vxor_vx_u8)(vtext_elem2, pattern_elem2, vl);
			TYPE_LMUL(vuint8) veq_elem3 = ADD_LMUL(__riscv_vxor_vx_u8)(vtext_elem3, pattern_elem3, vl);
			TYPE_LMUL(vuint8) veq_elem4 = ADD_LMUL(__riscv_vxor_vx_u8)(vtext_elem4, pattern_elem4, vl);
			TYPE_LMUL(vuint8) veq_first = ADD_LMUL(__riscv_vor_vv_u8)(veq_elem1, veq_elem2, vl);
			TYPE_LMUL(vuint8) veq_last = ADD_LMUL(__riscv_vor_vv_u8)(veq_elem3, veq_elem4, vl);
			TYPE_LMUL(vuint8) veq = ADD_LMUL(__riscv_vor_vv_u8)(veq_first, veq_last, vl);
            
            uint8_t vcmph[512];
            ADD_LMUL(__riscv_vse8_v_u8)(vcmph, veq, vl);
			
			for(int t = 0; t < vl; t++) {
                if (vcmph[t] == 0) {
					res += ADD_LMUL(compare_strings_vector8)(data, i, j + t, len);
				}
			}
		}
		freq[i] = res;
	});
}

#endif
