# Kokkos must be pre-installed
# -DKOKKOS_ROOT must be set!

# build Kokkos 4.7.04 with the following command for NVidia A100 and Cuda 11:
# cmake .. -DKokkos_ENABLE_CUDA=ON -DKokkos_ENABLE_CUDA_LAMBDA=ON -DKokkos_ARCH_AMPERE80=ON -DKokkos_ENABLE_SERIAL=ON -DCMAKE_CXX_COMPILER=$(pwd)/bin/nvcc_wrapper

set(CMAKE_CXX_COMPILER ${KOKKOS_ROOT}/bin/nvcc_wrapper)

set(PLATFORM "nvidia_gpu")
set(FRAMEWORK "kokkos")
