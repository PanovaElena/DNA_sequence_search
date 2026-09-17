set(SOURCES_EXPRS "")
set(LOCAL_INCLUDE_DIRS "")

if ("${FRAMEWORK}" STREQUAL "openmp")
    list(APPEND LOCAL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/openmp/include")
    list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/openmp/scalar/*.cpp")
    if ("${PLATFORM}" STREQUAL "x86")
        list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/openmp/vector_avx512/*.cpp")
    elseif ("${PLATFORM}" STREQUAL "riscv")
        list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/openmp/vector_rvv/*.cpp")
    endif()
elseif ("${FRAMEWORK}" STREQUAL "kokkos")
    list(APPEND LOCAL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/kokkos/include")
    list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/kokkos/scalar/*.cpp")
    list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/kokkos/vector/*.cpp")
elseif ("${FRAMEWORK}" STREQUAL "sycl")
    list(APPEND LOCAL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/sycl/include")
    if ("${PLATFORM}" STREQUAL "x86" OR "${PLATFORM}" STREQUAL "nvidia_gpu")
        list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/sycl/scalar/*.cpp")
        if (USE_SYCL_ESIMD)
            list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/sycl/vector/*.cpp")
        endif()
    endif()
elseif ("${FRAMEWORK}" STREQUAL "caf" AND "${PLATFORM}" STREQUAL "x86")
    list(APPEND LOCAL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/caf/include")
    list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/caf/scalar/*.cpp")
elseif ("${FRAMEWORK}" STREQUAL "cuda" AND "${PLATFORM}" STREQUAL "nvidia_gpu")
    list(APPEND LOCAL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/cuda/include")
    list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/cuda/scalar/*.cu")
    list(APPEND SOURCES_EXPRS "${CMAKE_CURRENT_SOURCE_DIR}/cuda/vector/*.cu")
endif()

file(GLOB SOURCES ${SOURCES_EXPRS})

# common libary for unit tests
add_library(most_common_string_lib OBJECT ${SOURCES})

target_include_directories(most_common_string_lib
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${LOCAL_INCLUDE_DIRS}
)

target_link_libraries(most_common_string_lib
PUBLIC
    FrameworkConfiguration
)

# libraries for performance tests, separate kernels
foreach(SOURCE ${SOURCES})
    get_filename_component(TARGET "${SOURCE}" NAME_WE)
    add_library(${TARGET} OBJECT ${SOURCE})
    
    target_include_directories(${TARGET}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${LOCAL_INCLUDE_DIRS}
    )
    
    target_link_libraries(${TARGET}
    PUBLIC
        FrameworkConfiguration
    )
endforeach()

# kernels for CAF
if ("${FRAMEWORK}" STREQUAL "caf")
    set(FORTRAN_SOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/caf/scalar/fortran/*.f90")
    file(GLOB FORTRAN_SOURCES ${FORTRAN_SOURCES_PATH})
    
    foreach(SOURCE ${FORTRAN_SOURCES})
        get_filename_component(TARGET "${SOURCE}" NAME_WE)
        add_executable(${TARGET} ${SOURCE})
        
        set_target_properties(${TARGET}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${FORTRAN_BINARY_PATH}"
        )
        
        target_include_directories(${TARGET}
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/caf/include"
        )
    endforeach()
endif()
