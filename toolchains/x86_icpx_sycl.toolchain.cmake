set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_CXX_COMPILER icpx)
set(CMAKE_C_COMPILER   icx)

set(CMAKE_CXX_FLAGS_INIT "-fsycl -fsycl-targets=spir64_x86_64")

set(PLATFORM "x86")
set(FRAMEWORK "sycl")
