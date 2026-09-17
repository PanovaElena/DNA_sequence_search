#ifndef KERNEL_LAUNCHER_HPP
#define KERNEL_LAUNCHER_HPP

#include "algorithms.hpp"

#if (defined(__CUDA__))

template<typename Kernel>
forceinline void run_search_kernel(std::vector<uint32_t>& freq_,
    const std::string& input_file, const uint32_t len, Kernel kernel)
{
    std::vector<uint8_t> datav;
    std::vector<uint32_t> freqv;
    size_t size = 0;
    prepare_data(input_file, datav, freqv, size);
    
    uint8_t* data = nullptr;
    uint32_t* freq = nullptr;
    cudaMallocManaged((void**)&data, datav.size()*sizeof(uint8_t));
    cudaMallocManaged((void**)&freq, freqv.size()*sizeof(uint32_t));
    for (size_t i = 0; i < datav.size(); i++) data[i] = datav[i];
    for (size_t i = 0; i < freqv.size(); i++) freq[i] = freqv[i];
    
    float time_ms = 0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    
    // run kernel    
    const int THREADS_PER_BLOCK = 256;
    const int N_BLOCKS = (size - len + THREADS_PER_BLOCK) / THREADS_PER_BLOCK;
    kernel<<< N_BLOCKS, THREADS_PER_BLOCK >>>(data, freq, len, size);
    cudaDeviceSynchronize();
    
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time_ms, start, stop);
    
    freq_.resize(size);
    for (size_t i = 0; i < size; ++i) freq_[i] = freq[i];
    
    cudaFree(freq);
    cudaFree(data);
    
    print_result(size, len, time_ms/1000.0f);
}

#endif // __CUDA__

#endif // KERNEL_LAUNCHER_HPP
