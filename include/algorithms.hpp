#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <cmath>
#include <random>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <unordered_map>

#if defined(__KOKKOS__)
    #include <Kokkos_Core.hpp>
    #include <Kokkos_SIMD.hpp>
#endif

#if defined(__SYCL__)
    #include <sycl/sycl.hpp>
    #if defined(__X86__)
        #include <sycl/ext/intel/esimd.hpp>
    #endif
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define forceinline __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
    #define forceinline __forceinline
#elif defined(__CUDA__)
    #define forceinline __forceinline__
#else
    #define forceinline inline
#endif

#if (defined(__OPENMP__) || defined(__KOKKOS__) || defined(__SYCL__) || defined(__CUDA__) || defined(__CAF__))

void naive_scalar(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void rabin_karp_rolling_hash_scalar(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void rabin_karp_SWAR_scalar(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void rabin_karp_SWAR2_scalar(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void hash3_scalar(std::vector<uint32_t>& freq, const std::string& input_file, const int len);

#endif

#if (defined(__OPENMP__) || defined(__KOKKOS__) || (defined(__SYCL__) && defined(__ESIMD__)) || defined(__CUDA__))

void naive_vector(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void rabin_karp_rolling_hash_vector(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void rabin_karp_SWAR_vector(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void rabin_karp_SWAR2_vector(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void hash3_vector(std::vector<uint32_t>& freq, const std::string& input_file, const int len);

#endif

#if (defined(__OPENMP__) && defined(__X86__))

void rabin_karp_rolling_hash_block_vector(std::vector<uint32_t>& freq, const std::string& input_file, const int len);
void naive_block_vector(std::vector<uint32_t>& freq, const std::string& input_file, const int len);

#endif


// auxuliary functions

inline void prepare_data(const std::string& input_file,
    std::vector<uint8_t>& data, std::vector<uint32_t>& freq, size_t& size)
{
    std::ifstream fin(input_file);
    std::string data_;
    fin >> data_;
    size=data_.size();

    std::unordered_map<uint8_t, uint8_t> mapSymbToCode = {
        {'A', (uint8_t)0}, {'C', (uint8_t)1}, {'G', (uint8_t)2}, {'T', (uint8_t)3}
    };
    data.resize(size + 4097);
	freq.resize(size + 4097);
    for (int i = 0; i < size; ++i){
        data[i] = mapSymbToCode[data_[i]];
        freq[i] = 0.0;
    }
    for (int i = size; i < size + 4097; ++i){
        data[i] = 5;
        freq[i] = 0.0;
    }
}

inline void print_result(size_t size, int len, double time_s) {
    std::cout << "text size=" << size << ", pattern size=" << len << ", time=" << time_s << " s" << std::endl;
}

#endif // ALGORITHMS_HPP
