#include "algorithms.hpp"

void host_naive(std::vector<uint32_t>& freq, const std::string& input_file, const uint32_t len_) {
    std::ifstream fin(input_file);
    std::string data_;
    fin >> data_;
    size_t size=data_.size();
    size_t len=len_;

    std::unordered_map<uint8_t, uint8_t> mapSymbToCode = {
        {'A', (uint8_t)0}, {'C', (uint8_t)1}, {'G', (uint8_t)2}, {'T', (uint8_t)3}
    };
    freq.resize(size);
    std::vector<uint8_t> datav(size);
    uint8_t* data = datav.data();
    for(int i=0;i<size;++i){
        data[i]=mapSymbToCode[data_[i]];
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < size-len+1; ++i){
        uint32_t res=0;
        for(int j = 0; j < size-len+1; ++j){
            int is_eq = 1;
            for(int k = 0; k < len && is_eq; ++k) {
                if (data[i + k] != data[j + k]) {
                    is_eq = 0;
                }
            }
            res += is_eq;
        }
        freq[i] = res;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    print_result(freq.size(), len, (std::chrono::duration<double>(stop - start)).count());
    return;
}