#!/bin/bash

# Lists of available platforms and technologies
PLATFORMS=("x86" "riscv" "nvidia_gpu")
FRAMEWORKS=("openmp" "kokkos" "sycl" "caf" "cuda")

show_help() {
    echo "Usage: $0 -p <platform> -f <framework> [-k <path_to_kokkos>] [-a <on/off>]"
    echo ""
    echo "Available platforms:"
    printf "  %s\n" "${PLATFORMS[@]}"
    echo ""
    echo "Available frameworks:"
    printf "  %s\n" "${FRAMEWORKS[@]}"
    echo ""
    echo "Example: $0 -p x86 -f openmp"
    echo ""
    echo "Option '-a' enables AOT compilation for SYCL on x86 CPU (default: on)"
}

if [ $# -eq 0 ]; then
    show_help
    exit 1
fi

PLATFORM=""
FRAMEWORK=""
KOKKOS_ROOT=""
AOT="on"

while getopts "p:f:k:a:h" opt; do
    case $opt in
        p)
            PLATFORM="$OPTARG"
            ;;
        f)
            FRAMEWORK="$OPTARG"
            ;;
        k)
            KOKKOS_ROOT="$OPTARG"
            ;;
        a)
            AOT="$OPTARG"
            ;;
        h)
            show_help
            exit 0
            ;;
        :)
            echo "Option -$OPTARG requires an argument" >&2
            show_help
            exit 1
            ;;
    esac
done

# Validate platform exists in the list
platform_valid=false
for p in "${PLATFORMS[@]}"; do
    if [ "$p" = "$PLATFORM" ]; then
        platform_valid=true
        break
    fi
done

if [ "$platform_valid" = false ]; then
    echo "Error: Platform '$PLATFORM' not found in the available list" >&2
    echo "Available platforms: ${PLATFORMS[*]}" >&2
    exit 1
fi

# Validate technology exists in the list
framework_valid=false
for t in "${FRAMEWORKS[@]}"; do
    if [ "$t" = "$FRAMEWORK" ]; then
        framework_valid=true
        break
    fi
done

if [ "$framework_valid" = false ]; then
    echo "Error: Framework '$FRAMEWORK' not found in the available list" >&2
    echo "Available frameworks: ${FRAMEWORKS[*]}" >&2
    exit 1
fi

# Generate cmake
TOOLCHAIN_FILE=
CMAKE_LINE=()

case "${PLATFORM}:${FRAMEWORK}:${AOT}" in
    x86:caf:*)
        TOOLCHAIN_FILE="$(pwd)/toolchains/x86_ifx_caf.toolchain.cmake"
        ;;
    x86:openmp:*)
        TOOLCHAIN_FILE="$(pwd)/toolchains/x86_icpx_openmp.toolchain.cmake"
        ;;
    x86:kokkos:*)
        TOOLCHAIN_FILE="$(pwd)/toolchains/x86_icpx_kokkos.toolchain.cmake"
        CMAKE_LINE+=("-DKOKKOS_ROOT=$KOKKOS_ROOT")
        ;;
    x86:sycl:on)
        TOOLCHAIN_FILE="$(pwd)/toolchains/x86_icpx_sycl_aot.toolchain.cmake"
        ;;
    x86:sycl:off)
        TOOLCHAIN_FILE="$(pwd)/toolchains/x86_icpx_sycl.toolchain.cmake"
        ;;
    x86:sycl:*)
        echo "ERROR: AOT must be 'on' or 'off' for SYCL on x86!"
        exit 1
        ;;
    nvidia_gpu:cuda:*)
        TOOLCHAIN_FILE="$(pwd)/toolchains/nvidia_a100_cuda.toolchain.cmake"
        ;;
    nvidia_gpu:kokkos:*)
        TOOLCHAIN_FILE="$(pwd)/toolchains/nvidia_a100_kokkos.toolchain.cmake"
        CMAKE_LINE+=("-DKOKKOS_ROOT=$KOKKOS_ROOT")
        ;;
    nvidia_gpu:sycl:on)
        TOOLCHAIN_FILE="$(pwd)/toolchains/nvidia_a100_icpx_sycl_aot.toolchain.cmake"
        CMAKE_LINE+=("-DGCC_TOOLCHAIN_PATH=$(dirname "$(which gcc)")/../")
        ;;
    nvidia_gpu:sycl:off)
        echo "ERROR: AOT must be 'on' for SYCL on NVIDIA GPU!"
        exit 1
        ;;
    riscv:openmp:*)
        TOOLCHAIN_FILE="$(pwd)/toolchains/riscv_gcc_openmp.toolchain.cmake"
        ;;
    riscv:kokkos:*)
        TOOLCHAIN_FILE="$(pwd)/toolchains/riscv_gcc_kokkos.toolchain.cmake"
        CMAKE_LINE+=("-DKOKKOS_ROOT=$KOKKOS_ROOT")
        ;;
    *)
        echo "ERROR: configuration 'platform=$PLATFORM' and 'framework=$FRAMEWORK' is not supported!"
        exit 1
        ;;
esac

BUILD_DIR="./build_${PLATFORM}_${FRAMEWORK}"
if [ "$FRAMEWORK" = "sycl" ]; then
    if [ "$AOT" = "on" ]; then
        BUILD_DIR="./build_${PLATFORM}_${FRAMEWORK}_aot"
    fi
fi    

mkdir -p ./build
cd ./build
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake ../../ -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} "${CMAKE_LINE[@]}" -DCMAKE_BUILD_TYPE=Release
cmake --build ./ -j --config=Release

cd ../../
