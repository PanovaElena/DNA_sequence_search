#ifndef KERNEL_LAUNCHER_HPP
#define KERNEL_LAUNCHER_HPP

#include "algorithms.hpp"

#if (defined(__OPENMP__))

template<typename Kernel>
forceinline void run_search_kernel(std::vector<uint32_t>& freq_,
    const std::string& input_file, const int len, const Kernel& kernel)
{
    std::vector<uint8_t> datav;
    std::vector<uint32_t> freqv;
    size_t size = 0;
    prepare_data(input_file, datav, freqv, size);
    
    uint8_t* data = datav.data();
    uint32_t* freq = freqv.data();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // run kernel    
#pragma omp parallel for
    for(int i = 0; i < size - len + 1; ++i) {
        kernel(i, data, freq, len, size);
    }
    
    auto stop = std::chrono::high_resolution_clock::now();
    
    freq_.resize(size);
    for (size_t i = 0; i < size; ++i) freq_[i] = freq[i];
    
    print_result(size, len, (std::chrono::duration<double>(stop - start)).count());
}

#endif // __OPENMP__

#endif // KERNEL_LAUNCHER_HPP
