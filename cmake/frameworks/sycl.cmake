# Framework configuration for Intel SYCL

add_library(FrameworkConfiguration INTERFACE)
target_compile_definitions(FrameworkConfiguration INTERFACE __SYCL__)
