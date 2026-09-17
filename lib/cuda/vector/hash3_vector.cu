#include "algorithms.hpp"
#include "compare.hpp"
#include "kernel_launcher.hpp"

#if (defined(__CUDA__))

__global__ void search_kernel_hash3_vector(uint8_t* data, uint32_t* freq, int len, int size) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= size - len + 1) return;
    
    int res=0;
    int sh1;
    int shift[64];
    for (int j = 0; j < 64; ++j) shift[j] = len-2;
    for(int j=2;j<len-1;++j){
        int ind=data[i+j-2]*16+data[i+j-1]*4+data[i+j];
        shift[ind]=len-1-j;
    }

    int ind=data[i+len-3]*16+data[i+len-2]*4+data[i+len-1];
    sh1=shift[ind];
    shift[ind]=0;
    
    if (!sh1) sh1=1;

    int j=len-1;

    for(;;){
        int sh=1;
        while (sh && j<size) {
            int ind=data[j-2]*16+data[j-1]*4+data[j];
            sh=shift[ind];
            j+=sh;
        } 
        if (j<size){
            res += compare_strings_vector(data, i, j-len+1, len);
            j+=sh1;
        }
        else{
            break;
        }
    }
    freq[i]=res;
}

void hash3_vector(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_search_kernel(freq_, input_file, len, search_kernel_hash3_vector);
}

#endif
