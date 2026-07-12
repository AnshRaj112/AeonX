add_library(myc_source STATIC
    src/source_buffer.cpp
    src/source_manager.cpp
)

target_include_directories(myc_source
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_source PUBLIC myc_shared myc_diagnostics)

myc_set_target_standard(myc_source)
myc_set_warnings(myc_source)

add_library(myc::source ALIAS myc_source)
