# -DGCC_TOOLCHAIN_PATH must be set!
# run $(dirname "$(which gcc)") to determine the toolchain

set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_FLAGS_INIT "-fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 --gcc-toolchain=${GCC_TOOLCHAIN_PATH}")

set(PLATFORM "nvidia_gpu")
set(FRAMEWORK "sycl")
