add_library(myc_source STATIC
    src/encoding.cpp
    src/file_loader.cpp
    src/future_include_resolver.cpp
    src/line_table.cpp
    src/source_buffer.cpp
    src/source_file.cpp
    src/source_manager.cpp
    src/source_span.cpp
)

target_include_directories(myc_source
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_source PUBLIC myc_shared myc_diagnostics myc_common)

myc_set_target_standard(myc_source)
myc_set_warnings(myc_source)

add_library(myc::source ALIAS myc_source)
