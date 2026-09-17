function(add_perf_test SOURCE)
    get_filename_component(LIB_NAME ${SOURCE} NAME_WE)
    set(TARGET "${LIB_NAME}_perf")
    
    add_executable(${TARGET} ${SOURCE})

    target_include_directories(${TARGET}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/perf/include
    )
    
    if(TARGET ${LIB_NAME})
        target_link_libraries(${TARGET} PRIVATE ${LIB_NAME})
    endif()
    target_link_libraries(${TARGET} PRIVATE FrameworkConfiguration)
endfunction()

file(GLOB SOURCES_SCALAR "${CMAKE_CURRENT_SOURCE_DIR}/src/scalar/*.cpp")
file(GLOB SOURCES_VECTOR "${CMAKE_CURRENT_SOURCE_DIR}/src/vector/*.cpp")

foreach(SOURCE ${SOURCES_SCALAR})
    add_perf_test(${SOURCE})
endforeach()

if (NOT (FRAMEWORK STREQUAL "caf") AND NOT (FRAMEWORK STREQUAL "sycl" AND NOT USE_SYCL_ESIMD))
    foreach(SOURCE ${SOURCES_VECTOR})
        add_perf_test(${SOURCE})
    endforeach()
endif()
