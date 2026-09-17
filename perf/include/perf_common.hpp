#ifndef PERF_COMMON_HPP
#define PERF_COMMON_HPP

#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <string>
#include <cstdint>

#include "algorithms.hpp"

inline void initialize_env(int argc, char **argv) {
#ifdef __KOKKOS__
    Kokkos::initialize(argc, argv);
#endif
}

inline void finalize_env() {
#ifdef __KOKKOS__
    Kokkos::finalize();
#endif
}

#endif // PERF_COMMON_HPP
