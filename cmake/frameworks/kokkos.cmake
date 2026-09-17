# Framework configuration for Kokkos
# KOKKOS_ROOT must be set

add_library(FrameworkConfiguration INTERFACE)

if ("${KOKKOS_ROOT}" STREQUAL "")
    message(WARNING "KOKKOS_ROOT is not found")
endif()

find_package(Kokkos REQUIRED PATHS ${KOKKOS_ROOT} NO_DEFAULT_PATH)

target_include_directories(FrameworkConfiguration INTERFACE ${Kokkos_INCLUDE_DIRS})
target_link_libraries(FrameworkConfiguration INTERFACE Kokkos::kokkos)

target_compile_definitions(FrameworkConfiguration INTERFACE __KOKKOS__)

