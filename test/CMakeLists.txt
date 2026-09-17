file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(most_common_string_test ${SOURCES})

target_include_directories(most_common_string_test
PRIVATE
    ${PROJECT_SOURCE_DIR}/3rd-party/gtest/googletest/include
    ${PROJECT_SOURCE_DIR}/test/include
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(most_common_string_test
PRIVATE
    most_common_string_lib
    GTest::gtest
    GTest::gtest_main
    FrameworkConfiguration
)
