# MycOptions.cmake — Global project options and C++ standard configuration.

option(MYC_BUILD_TESTS "Build unit tests" ON)
option(MYC_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(MYC_ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
option(MYC_ENABLE_LTO "Enable link-time optimization in Release builds" OFF)

set(MYC_CXX_STANDARD 23 CACHE STRING "C++ standard to use")
set_property(CACHE MYC_CXX_STANDARD PROPERTY STRINGS 23)

function(myc_set_target_standard target)
    target_compile_features(${target} PUBLIC cxx_std_${MYC_CXX_STANDARD})
    set_target_properties(${target} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
    )
endfunction()

function(myc_export_target target)
    target_include_directories(${target}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )
endfunction()
