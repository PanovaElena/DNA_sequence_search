#include "perf_common.hpp"

int main(int argc, char** argv) {
    initialize_env(argc, argv);
    
    if (argc < 2) {
        std::cout << "Error args: genome file name expected" << std::endl;
        return 0;
    }

#if (defined(__OPENMP__) && (defined(__X86__) || defined(__RISCV__)))
    std::string path = argv[1];
    std::vector<uint32_t> freq;

    naive_block_vector(freq, path, 128);
#endif

    finalize_env();

    return 0;
}
