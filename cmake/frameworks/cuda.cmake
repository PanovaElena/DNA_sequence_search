# Framework configuration for Cuda

add_library(FrameworkConfiguration INTERFACE)
target_compile_definitions(FrameworkConfiguration INTERFACE __CUDA__)
