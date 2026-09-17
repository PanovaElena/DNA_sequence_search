#ifndef KERNEL_LAUNCHER_HPP
#define KERNEL_LAUNCHER_HPP

#include "algorithms.hpp"

#if (defined(__SYCL__))

template<typename Kernel>
forceinline void run_search_kernel(std::vector<uint32_t>& freq_,
    const std::string& input_file, const uint32_t len, Kernel kernel)
{
    std::vector<uint8_t> datav;
    std::vector<uint32_t> freqv;
    size_t size = 0;
    prepare_data(input_file, datav, freqv, size);
    
    auto exception_handler = [](sycl::exception_list e_list) {
        for (std::exception_ptr const& e : e_list) {
            try {
                std::rethrow_exception(e);
            }
            catch (std::exception const& e) {
                std::cout << "Failure" << std::endl;
                std::terminate();
            }
        }
    };
    sycl::queue q(sycl::default_selector_v, exception_handler);
    
    try {
        
        uint8_t* data = sycl::malloc_shared<uint8_t>(datav.size(), q);
        uint32_t* freq = sycl::malloc_shared<uint32_t>(freqv.size(), q);
        for (size_t i = 0; i < datav.size(); i++) data[i] = datav[i];
        for (size_t i = 0; i < freqv.size(); i++) freq[i] = freqv[i];
        
        auto start = std::chrono::steady_clock::now();
        
        // run kernel
        q.submit([&](auto& handler) {
            handler.parallel_for(sycl::range<1>(size - len + 1), [=](sycl::item<1> i) {
                kernel(i.get_id(0), data, freq, len, size);
            });
        });
        q.wait_and_throw();
        
        auto stop = std::chrono::steady_clock::now();
        
        freq_.resize(size);
        for (size_t i = 0; i < size; ++i) freq_[i] = freq[i];
        
        sycl::free(data, q);
        sycl::free(freq, q);
        
        print_result(size, len, (std::chrono::duration<double>(stop - start)).count());  
    }
    catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
    }
}

#endif // __SYCL__

#endif // KERNEL_LAUNCHER_HPP
