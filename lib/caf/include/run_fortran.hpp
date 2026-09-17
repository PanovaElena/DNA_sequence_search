#pragma once
#include "algorithms.hpp"

#if defined(__CAF__)

// CAF implies a few separate processes (images)
forceinline void run_fortran(std::vector<uint32_t>& freq,
    const std::string& input_file, const int len, const std::string& fprogram_name) {
    
    const std::string binary_path = std::string(__FORTRAN_BINARY_PATH__) + "/";
    const std::string output_file_name = fprogram_name + "_fortran_output.bin";
    
    // run CAF subprocesses
    std::string command = binary_path + fprogram_name + " " + input_file + " " +
        std::to_string(len) + " " + binary_path + output_file_name;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Error: cannot start fortran program, " << result << std::endl;
        return;
    }
    
    // read output file
    std::ifstream output_file(binary_path + output_file_name, std::ios::binary);    
    if (!output_file.is_open()) {
        throw std::runtime_error("Cannot open output file: " + (binary_path + output_file_name));
    }
    
    double time = 0.0;
    output_file.read(reinterpret_cast<char*>(&time), sizeof(double));
    size_t size = 0;
    output_file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    freq.resize(size);
    output_file.read(reinterpret_cast<char*>(freq.data()), size * sizeof(uint32_t));
    
    print_result(size, len, time);
}

#endif
