set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_CXX_COMPILER icpx)
set(CMAKE_C_COMPILER   icx)

set(CMAKE_Fortran_COMPILER ifx)

set(CMAKE_CXX_FLAGS_INIT "-march=icelake-server -g -O3 -qopt-zmm-usage=high")
set(CMAKE_C_FLAGS_INIT   "-march=icelake-server -g -O3 -qopt-zmm-usage=high")

set(CMAKE_Fortran_FLAGS_INIT "-coarray -march=icelake-server -O3 -fPIC")

set(PLATFORM "x86")
set(FRAMEWORK "caf")
