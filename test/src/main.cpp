#include "test_common.hpp"

void initialize_env(int argc, char **argv) {
#ifdef __KOKKOS__
    Kokkos::initialize(argc, argv);
#endif
}

void finalize_env() {
#ifdef __KOKKOS__
    Kokkos::finalize();
#endif
}

int main(int argc, char **argv) {
    srand(time(NULL));
    
    initialize_env(argc, argv);
    
    ::testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    
    finalize_env();
    
    return res;
}
