add_library(myc_diagnostics STATIC
    src/diagnostic.cpp
    src/diagnostic_engine.cpp
)

target_include_directories(myc_diagnostics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_diagnostics PUBLIC myc_shared)

myc_set_target_standard(myc_diagnostics)
myc_set_warnings(myc_diagnostics)

add_library(myc::diagnostics ALIAS myc_diagnostics)
