#include "perf_common.hpp"

int main(int argc, char** argv) {
    initialize_env(argc, argv);
    
    if (argc < 2) {
        std::cout << "Error args: genome file name expected" << std::endl;
        return 0;
    }
    
    std::string path = argv[1];
    std::vector<uint32_t> freq;
    
    rabin_karp_rolling_hash_vector(freq, path, 128);

    finalize_env();

    return 0;
}
