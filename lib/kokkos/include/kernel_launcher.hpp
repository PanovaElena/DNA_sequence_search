#ifndef KERNEL_LAUNCHER_HPP
#define KERNEL_LAUNCHER_HPP

#include "algorithms.hpp"

#if (defined(__KOKKOS__))

template<typename Kernel>
forceinline void run_search_kernel(std::vector<uint32_t>& freq_,
    const std::string& input_file, const uint32_t len, const Kernel& kernel)
{
    std::vector<uint8_t> datav;
    std::vector<uint32_t> freqv;
    size_t size = 0;
    prepare_data(input_file, datav, freqv, size);
    
    MyViewStr data("text", datav.size());
	MyViewFreq freq("freq", freqv.size());
    for (size_t i = 0; i < datav.size(); i++) data[i] = datav[i];
    for (size_t i = 0; i < freqv.size(); i++) freq[i] = freqv[i];
    
    Kokkos::Timer timer;
    timer.reset();
    
    // run kernel    
    Kokkos::parallel_for("pattern_search", size - len + 1, KOKKOS_LAMBDA(int i) {
		kernel(i, data, freq, len, size);
	});
    Kokkos::fence();
	
    double stop = timer.seconds();
    
    freq_.resize(size);
    for (size_t i = 0; i < size; ++i) freq_[i] = freq[i];
    
    print_result(size, len, stop);
}

#endif // __KOKKOS__

#endif // KERNEL_LAUNCHER_HPP
