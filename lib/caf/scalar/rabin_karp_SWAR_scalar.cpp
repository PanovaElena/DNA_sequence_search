#include "run_fortran.hpp"

#if (defined(__X86__) && defined(__CAF__))

void rabin_karp_SWAR_scalar(std::vector<uint32_t>& freq_, const std::string& input_file, const int len) {
    run_fortran(freq_, input_file, len, "rabin_karp_SWAR_scalar_fortran");
}

#endif
