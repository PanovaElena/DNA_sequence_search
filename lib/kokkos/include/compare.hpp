#ifndef COMPARE_HPP
#define COMPARE_HPP

#include "algorithms.hpp"

#if (defined(__KOKKOS__))

#include <Kokkos_Core.hpp>
#include <Kokkos_SIMD.hpp>

using MyViewStr = Kokkos::View<uint8_t*, Kokkos::SharedSpace>;
using MyViewFreq = Kokkos::View<uint32_t*, Kokkos::SharedSpace>;

KOKKOS_INLINE_FUNCTION
int compare_strings_scalar(const MyViewStr& data, int i, int j, const size_t len) {
    int is_eq = 1;
    for(int k = 0; k < len && is_eq; ++k) {
        if (data[i + k] != data[j + k]) {
            is_eq = 0;
        }
    }
    return is_eq;
}

namespace simd = Kokkos::Experimental;
using simd_type = simd::simd<uint32_t>;

// vector load
// CPU: do nothing (compiler optimization)
// GPU: load 1 scalar element uint32_t (4 elements uint8_t)
KOKKOS_INLINE_FUNCTION
void load_simd(const MyViewStr& data, int i, simd_type& vr) {
    constexpr size_t simd_len_int32 = simd_type::size();
    
    uint32_t tmp[simd_len_int32];
    #pragma unroll
    for (int t = 0; t < simd_len_int32; t++) {
        int idx = i + t * 4;
        tmp[t] = static_cast<uint32_t>(data[idx]) |
            (static_cast<uint32_t>(data[idx + 1]) << 8) |
            (static_cast<uint32_t>(data[idx + 2]) << 16) |
            (static_cast<uint32_t>(data[idx + 3]) << 24);
    }
    
    vr = simd_type(tmp, simd::element_aligned_tag());
}

KOKKOS_INLINE_FUNCTION
int compare_strings_vector(const MyViewStr& data, int i, int j, const size_t len) {
    constexpr size_t simd_len_bytes = simd_type::size() * 4;
    const size_t simd_cycles = len / simd_len_bytes;
    const size_t simd_cycles_bytes = simd_cycles*simd_len_bytes;
    
    int is_eq = 1;
    
	for(int k = 0; k < simd_cycles_bytes && is_eq; k += simd_len_bytes) {
        simd_type vpattern_1, vpattern_2;
        load_simd(data, i + k, vpattern_1);
        load_simd(data, j + k, vpattern_2);
        
        is_eq = simd::all_of(vpattern_1 == vpattern_2);
	}
	
	for(int k = simd_cycles_bytes; k < len && is_eq; ++k){
		if (data[i+k] != data[j+k]){
			is_eq = 0;
		}
	}
    
    return is_eq;
}

#endif // __KOKKOS__

#endif // COMPARE_HPP
