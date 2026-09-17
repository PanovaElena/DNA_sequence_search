# Framework configuration for OpenMP

add_library(FrameworkConfiguration INTERFACE)

target_compile_definitions(FrameworkConfiguration INTERFACE __CAF__)
target_compile_definitions(FrameworkConfiguration INTERFACE __FORTRAN_BINARY_PATH__="${FORTRAN_BINARY_PATH}")
