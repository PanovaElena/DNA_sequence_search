# Framework configuration for OpenMP

add_library(FrameworkConfiguration INTERFACE)

find_package(OpenMP REQUIRED)

target_link_libraries(FrameworkConfiguration INTERFACE OpenMP::OpenMP_CXX)

target_compile_definitions(FrameworkConfiguration INTERFACE __OPENMP__)
