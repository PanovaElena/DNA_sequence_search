set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Kokkos must be pre-installed
# -DKOKKOS_ROOT must be set!

# build Kokkos 4.7.04 with the following command for RISC-V (RVV 1.0) with GNU compiler:
# cmake .. -DCMAKE_CXX_COMPILER=riscv64-unknown-linux-gnu-g++ -DCMAKE_CXX_FLAGS="-march=rv64gcv -mabi=lp64d -O3" -DKokkos_ENABLE_OPENMP=ON -DKokkos_ENABLE_AGGRESSIVE_VECTORIZATION=ON -DKokkos_ARCH_RISCV_RVA22V=ON

set(CMAKE_CXX_COMPILER riscv64-unknown-linux-gnu-g++)
set(CMAKE_C_COMPILER   riscv64-unknown-linux-gnu-gcc)

set(CMAKE_CXX_FLAGS_INIT "-march=rv64gcv -mabi=lp64d -O3")
set(CMAKE_C_FLAGS_INIT   "-march=rv64gcv -mabi=lp64d -O3")

set(PLATFORM "riscv")
set(FRAMEWORK "kokkos")
