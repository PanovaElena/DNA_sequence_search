set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Kokkos must be pre-installed
# -DKOKKOS_ROOT must be set!

# build Kokkos 4.7.04 with the following command for x86 icelake-server and Intel Compiler:
# cmake .. -DCMAKE_CXX_COMPILER=icpx -DCMAKE_CXX_FLAGS="-march=icelake-server -O3 -qopt-zmm-usage=high" -DKokkos_ENABLE_OPENMP=ON -DKokkos_ENABLE_AGGRESSIVE_VECTORIZATION=ON -DKokkos_ARCH_ICX=ON

set(CMAKE_CXX_COMPILER icpx)
set(CMAKE_C_COMPILER   icx)

set(CMAKE_CXX_FLAGS_INIT "-march=icelake-server -O3 -qopt-zmm-usage=high")
set(CMAKE_C_FLAGS_INIT   "-march=icelake-server -O3 -qopt-zmm-usage=high")

set(PLATFORM "x86")
set(FRAMEWORK "kokkos")
