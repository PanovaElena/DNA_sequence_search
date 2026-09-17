#ifndef COMPARE_HPP
#define COMPARE_HPP

#include "algorithms.hpp"

#if (defined(__CUDA__))

#include <cuda.h>


__device__ __forceinline__ int compare_strings_scalar(uint8_t* data, int i, int j, int len) {
    int is_eq = 1;
    for (int k = 0; k < len && is_eq; ++k) {
        if (data[i + k] != data[j + k]) {
            is_eq = 0;
        }
    }
    return is_eq;
}


// block (4 uint8_t) load
__device__ __forceinline__ uint32_t cuda_load_int32(uint8_t* data, int i) {   
    uint32_t tmp = static_cast<uint32_t>(data[i]) |
            (static_cast<uint32_t>(data[i + 1]) << 8) |
            (static_cast<uint32_t>(data[i + 2]) << 16) |
            (static_cast<uint32_t>(data[i + 3]) << 24);
    return tmp;
}

__device__ __forceinline__ int compare_strings_vector(uint8_t* data, int i, int j, int len) {
    int bytes_in_uint32 = 4;
    int cycles = len / bytes_in_uint32;
    int cycles_bytes = cycles*bytes_in_uint32;
    
    int is_eq = 1;
    
	for (int k = 0; k < cycles_bytes && is_eq; k += bytes_in_uint32) {
        uint32_t vpattern_1 = cuda_load_int32(data, i + k);
        uint32_t vpattern_2 = cuda_load_int32(data, j + k);
        
        is_eq = vpattern_1 == vpattern_2;
	}
	
	for (int k = cycles_bytes; k < len && is_eq; ++k){
		if (data[i+k] != data[j+k]){
			is_eq = 0;
		}
	}
    
    return is_eq;
}

#endif

#endif // COMPARE_HPP
