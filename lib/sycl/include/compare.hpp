#ifndef COMPARE_HPP
#define COMPARE_HPP

#include "algorithms.hpp"

#if (defined(__SYCL__))

forceinline int compare_strings_scalar(uint8_t* data, int i, int j, const size_t len) {
    int is_eq = 1;
    for(int k = 0; k < len && is_eq; ++k) {
        if (data[i + k] != data[j + k]) {
            is_eq = 0;
        }
    }
    return is_eq;
}


#if (defined(__ESIMD__))

constexpr int simd_len_int32 = 16;  // TODO:fix

namespace simd = sycl::ext::intel::esimd;
using simd_type = simd::simd<uint32_t, simd_len_int32>;

// vector load
forceinline void load_simd(uint8_t* data, int i, simd_type& vr) {
    uint32_t tmp[simd_len_int32];
    
    #pragma unroll
    for (int t = 0; t < simd_len_int32; t++) {
        int idx = i + t * 4;
        tmp[t] = static_cast<uint32_t>(data[idx]) |
            (static_cast<uint32_t>(data[idx + 1]) << 8) |
            (static_cast<uint32_t>(data[idx + 2]) << 16) |
            (static_cast<uint32_t>(data[idx + 3]) << 24);
    }
    
    vr.copy_from(tmp, simd::element_aligned);
}

forceinline int compare_strings_vector(uint8_t* data, int i, int j, const size_t len) {
    
    constexpr size_t simd_len_bytes = simd_len_int32 * 4;
    const size_t simd_cycles = len / simd_len_bytes;
    const size_t simd_cycles_bytes = simd_cycles*simd_len_bytes;
    
    int is_eq = 1;
    
	for(int k = 0; k < simd_cycles_bytes && is_eq; k += simd_len_bytes) {
        simd_type vpattern_1, vpattern_2;
        load_simd(data, i + k, vpattern_1);
        load_simd(data, j + k, vpattern_2);
        
        auto mask = (vpattern_1 == vpattern_2);
        is_eq = mask.all(); 
	}
	
	for(int k = simd_cycles_bytes; k < len && is_eq; ++k){
		if (data[i+k] != data[j+k]){
			is_eq = 0;
		}
	}
    
    return is_eq;
}

#endif // __ESIMD__

#endif // __SYCL__

#endif // COMPARE_HPP
