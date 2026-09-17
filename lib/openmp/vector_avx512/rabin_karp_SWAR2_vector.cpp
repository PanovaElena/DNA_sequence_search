#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__X86__) && defined(__OPENMP__))

void rabin_karp_SWAR2_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len,
        [](int i, uint8_t* data, uint32_t* freq, size_t len, size_t size) {
        __m512i vzeroes = _mm512_set1_epi32(0);
        constexpr size_t simd_lenbytes = 64, simd_lenint32 = simd_lenbytes / 4;
        
        uint32_t res = 0;
        uint32_t pattern_elem1234 = *reinterpret_cast<uint32_t*>(&data[i]);
        __m512i vpattern_elem1234 = _mm512_set1_epi32(pattern_elem1234);
        
        for (int j = 0; j < size - len + 1; j += simd_lenbytes) {
            
            __m512i v0text_elem1234 = _mm512_loadu_epi32(&data[j+0]);
            __m512i v1text_elem1234 = _mm512_loadu_epi32(&data[j+1]);
            __m512i v2text_elem1234 = _mm512_loadu_epi32(&data[j+2]);
            __m512i v3text_elem1234 = _mm512_loadu_epi32(&data[j+3]);
            
            __m512i v0eq_elem1234 = _mm512_xor_si512(vpattern_elem1234, v0text_elem1234);
            __m512i v1eq_elem1234 = _mm512_xor_si512(vpattern_elem1234, v1text_elem1234);
            __m512i v2eq_elem1234 = _mm512_xor_si512(vpattern_elem1234, v2text_elem1234);
            __m512i v3eq_elem1234 = _mm512_xor_si512(vpattern_elem1234, v3text_elem1234);
            
            __mmask16 v0cmph = _mm512_cmpeq_epi32_mask(v0eq_elem1234, vzeroes);
            __mmask16 v1cmph = _mm512_cmpeq_epi32_mask(v1eq_elem1234, vzeroes);
            __mmask16 v2cmph = _mm512_cmpeq_epi32_mask(v2eq_elem1234, vzeroes);
            __mmask16 v3cmph = _mm512_cmpeq_epi32_mask(v3eq_elem1234, vzeroes);
            
            uint32_t bitmaskh0 = _cvtmask16_u32(v0cmph);
            uint32_t bitmaskh1 = _cvtmask16_u32(v1cmph);
            uint32_t bitmaskh2 = _cvtmask16_u32(v2cmph);
            uint32_t bitmaskh3 = _cvtmask16_u32(v3cmph);
            
            while (bitmaskh0) {
                int t = _tzcnt_u32(bitmaskh0);
                res += compare_strings_vector_64(data, i, j+0 + t*4, len);
                bitmaskh0 = _blsr_u32(bitmaskh0);
            }
            while (bitmaskh1) {
                int t = _tzcnt_u32(bitmaskh1);
                res += compare_strings_vector_64(data, i, j+1 + t*4, len);
                bitmaskh1 = _blsr_u32(bitmaskh1);
            } 
            while (bitmaskh2) {
                int t = _tzcnt_u32(bitmaskh2);
                res += compare_strings_vector_64(data, i, j+2 + t*4, len);
                bitmaskh2 = _blsr_u32(bitmaskh2);
            } 
            while (bitmaskh3) {
                int t = _tzcnt_u32(bitmaskh3);
                res += compare_strings_vector_64(data, i, j+3 + t*4, len);
                bitmaskh3 = _blsr_u32(bitmaskh3);
            }
        }
        freq[i] = res;
    });
}

#endif
