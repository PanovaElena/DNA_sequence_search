#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__X86__) && defined(__OPENMP__))

void rabin_karp_SWAR_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        __m512i vzeroes = _mm512_set1_epi32(0);
        constexpr size_t simd_lenbytes = 64;
		
		uint32_t res = 0;
		__m512i vpattern_elem1 = _mm512_set1_epi8(data[i]);
		__m512i vpattern_elem2 = _mm512_set1_epi8(data[i+1]);
		__m512i vpattern_elem3 = _mm512_set1_epi8(data[i+len-2]);
		__m512i vpattern_elem4 = _mm512_set1_epi8(data[i+len-1]);
		
		for (int j = 0; j < size - len + 1; j += simd_lenbytes) {
			__m512i vtext_elem1 = _mm512_loadu_epi8(&data[j]);
			__m512i vtext_elem2 = _mm512_loadu_epi8(&data[j+1]);
			__m512i vtext_elem3 = _mm512_loadu_epi8(&data[j+len-2]);
			__m512i vtext_elem4 = _mm512_loadu_epi8(&data[j+len-1]);
			
			__m512i veq_elem1 = _mm512_xor_si512(vtext_elem1, vpattern_elem1);
			__m512i veq_elem2 = _mm512_xor_si512(vtext_elem2, vpattern_elem2);
			__m512i veq_elem3 = _mm512_xor_si512(vtext_elem3, vpattern_elem3);
			__m512i veq_elem4 = _mm512_xor_si512(vtext_elem4, vpattern_elem4);
			__m512i veq_first = _mm512_or_si512(veq_elem1, veq_elem2);
			__m512i veq_last = _mm512_or_si512(veq_elem3, veq_elem4);
			__m512i veq = _mm512_or_si512(veq_first, veq_last);
			
			__mmask64 vcmph = _mm512_cmpeq_epi8_mask(veq, vzeroes);
			uint64_t bitmaskh = _cvtmask64_u64(vcmph);
            
            while (bitmaskh) {
                int t = _tzcnt_u64(bitmaskh);
                res += compare_strings_vector_64(data, i, j + t, len);
                bitmaskh = _blsr_u64(bitmaskh);
            }
		}
		freq[i] = res;
	});
}

#endif
