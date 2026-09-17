#ifndef COMPARE_HPP
#define COMPARE_HPP

#include "algorithms.hpp"

#if (defined(__OPENMP__))

forceinline int compare_strings_scalar(const uint8_t* data, int i, int j, const size_t len) {
    int is_eq = 1;
    for(int k = 0; k < len && is_eq; ++k) {
        if (data[i + k] != data[j + k]) {
            is_eq = 0;
        }
    }
    return is_eq;
}


#if (defined(__X86__))

#include <immintrin.h>

#define CREATE_COMPARE_FUNC(VLEN, INAME, MASKL, BITMASK)                                        \
forceinline int compare_strings_vector_##VLEN(uint8_t* data, int i, int j, const size_t len) {  \
                                                                                                \
    uint8_t* text = &data[i], *pattern = &data[j];                                              \
                                                                                                \
    constexpr size_t simd_len_bytes = VLEN;                                                     \
    const size_t simd_cycles = len / simd_len_bytes;                                            \
    const size_t simd_leftover = len - simd_cycles*simd_len_bytes;                              \
    constexpr size_t simd_len_int32 = simd_len_bytes / 4;                                       \
                                                                                                \
    int is_eq = 1;                                                                              \
                                                                                                \
    uint32_t* pattern_1_uint32 = reinterpret_cast<uint32_t*>(text);                             \
    uint32_t* pattern_2_uint32 = reinterpret_cast<uint32_t*>(pattern);                          \
                                                                                                \
    for(int k = 0; k < simd_cycles; ++k) {                                                      \
        auto vpattern_1 = _m##INAME##_loadu_epi32(pattern_1_uint32);                            \
        auto vpattern_2 = _m##INAME##_loadu_epi32(pattern_2_uint32);                            \
        auto vcmp = _m##INAME##_cmpeq_epu32_mask(vpattern_1, vpattern_2);                       \
                                                                                                \
        uint32_t bitmask = _cvtmask##MASKL##_u32(vcmp);                                         \
        if (bitmask != BITMASK##U) {                                                            \
            is_eq=0;                                                                            \
            break;                                                                              \
        }                                                                                       \
                                                                                                \
        pattern_1_uint32 += simd_len_int32;                                                     \
        pattern_2_uint32 += simd_len_int32;                                                     \
    }                                                                                           \
                                                                                                \
    uint8_t* pattern_1_uint8 = reinterpret_cast<uint8_t*>(pattern_1_uint32);                    \
    uint8_t* pattern_2_uint8 = reinterpret_cast<uint8_t*>(pattern_2_uint32);                    \
                                                                                                \
    for(int k = 0; k < simd_leftover && is_eq; ++k){                                            \
        if (*pattern_1_uint8 != *pattern_2_uint8){                                              \
            is_eq = 0;                                                                          \
            break;                                                                              \
        }                                                                                       \
        pattern_1_uint8++;                                                                      \
        pattern_2_uint8++;                                                                      \
    }                                                                                           \
                                                                                                \
    return is_eq;                                                                               \
}

CREATE_COMPARE_FUNC(64, m512, 16, 0x0000ffff)
CREATE_COMPARE_FUNC(32, m256, 8,  0x000000ff)
CREATE_COMPARE_FUNC(16, m,    8,  0x0000000f)

#endif // __X86__

#if (defined(__RISCV__))

#include <riscv_vector.h>
#include <cstring>

#define CREATE_COMPARE_FUNC8(LMUL, MASKL)                                                 \
forceinline int compare_strings_vector8m##LMUL(uint8_t* data, int i, int j, const size_t len) {  \
                                                                                          \
    uint8_t* text = &data[i], *pattern = &data[j];                                        \
    int is_eq = 1;                                                                        \
                                                                                          \
    size_t vl = __riscv_vsetvl_e8m##LMUL(len);                                            \
                                                                                          \
    for(int k = 0; k < len; k += vl) {                                                    \
        vl = __riscv_vsetvl_e8m##LMUL(len - k);                                           \
                                                                                          \
        vuint8m##LMUL##_t vpattern_1 = __riscv_vle8_v_u8m##LMUL(text, vl);                \
        vuint8m##LMUL##_t vpattern_2 = __riscv_vle8_v_u8m##LMUL(pattern, vl);             \
        auto vcmp = __riscv_vmsne_vv_u8m##LMUL##_b##MASKL(vpattern_1, vpattern_2, vl);    \
                                                                                          \
        auto first = __riscv_vfirst_m_b##MASKL(vcmp, vl);                                 \
        if (first != -1) {                                                                \
            is_eq = 0;                                                                    \
            break;                                                                        \
        }                                                                                 \
                                                                                          \
        text += vl;                                                                       \
        pattern += vl;                                                                    \
    }                                                                                     \
                                                                                          \
    return is_eq;                                                                         \
}

CREATE_COMPARE_FUNC8(1, 8)
CREATE_COMPARE_FUNC8(2, 4)
CREATE_COMPARE_FUNC8(4, 2)
CREATE_COMPARE_FUNC8(8, 1)

#define CREATE_LOAD_FUNC32(LMUL)                                                          \
forceinline void load_simd32m##LMUL(uint8_t* data, vuint32m##LMUL##_t& vr, size_t vl) {  \
    uint32_t tmp[512];                                                                    \
    std::memcpy(tmp, data, vl * sizeof(uint32_t));                                        \
    vr = __riscv_vle32_v_u32m##LMUL(tmp, vl);                                             \
}

CREATE_LOAD_FUNC32(1)
CREATE_LOAD_FUNC32(2)
CREATE_LOAD_FUNC32(4)
CREATE_LOAD_FUNC32(8)

#define CREATE_COMPARE_FUNC32(LMUL, MASKL)                                                 \
forceinline int compare_strings_vector32m##LMUL(uint8_t* data, int i, int j, const size_t len) {  \
                                                                                           \
    uint8_t* text = &data[i], *pattern = &data[j];                                         \
                                                                                           \
    const size_t len_int32 = len / 4;                                                      \
    const size_t leftover_bytes = len - len_int32*4;  /* in {0,1,2} */                     \
                                                                                           \
    int is_eq = 1;                                                                         \
                                                                                           \
    size_t vl = __riscv_vsetvl_e32m##LMUL(len_int32);                                      \
                                                                                           \
    for(int k = 0; k < len_int32; k += vl) {                                               \
        vl = __riscv_vsetvl_e32m##LMUL(len_int32 - k);                                     \
                                                                                           \
        vuint32m##LMUL##_t vpattern_1, vpattern_2;                                         \
        load_simd32m##LMUL(text, vpattern_1, vl);                                         \
        load_simd32m##LMUL(pattern, vpattern_2, vl);                                      \
        auto vcmp = __riscv_vmseq_vv_u32m##LMUL##_b##MASKL(vpattern_1, vpattern_2, vl);    \
                                                                                           \
        unsigned long count = __riscv_vcpop_m_b##MASKL(vcmp, vl);                          \
        if (count != vl) {                                                                 \
            is_eq = 0;                                                                     \
            break;                                                                         \
        }                                                                                  \
                                                                                           \
        text += vl*4;                                                                      \
        pattern += vl*4;                                                                   \
    }                                                                                      \
                                                                                           \
    for(int k = 0; k < leftover_bytes && is_eq; ++k){                                      \
        if (*text != *pattern) {                                                           \
            is_eq = 0;                                                                     \
            break;                                                                         \
        }                                                                                  \
        text++;                                                                            \
        pattern++;                                                                         \
    }                                                                                      \
                                                                                           \
    return is_eq;                                                                          \
}

CREATE_COMPARE_FUNC32(1, 32)
CREATE_COMPARE_FUNC32(2, 16)
CREATE_COMPARE_FUNC32(4, 8)
CREATE_COMPARE_FUNC32(8, 4)

#endif // __RISCV__

#endif // __OPENMP__

#endif // COMPARE_HPP
