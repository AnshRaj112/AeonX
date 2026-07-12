add_library(myc_diagnostics STATIC
    src/ansi_color.cpp
    src/diagnostic.cpp
    src/diagnostic_builder.cpp
    src/diagnostic_emitter.cpp
    src/diagnostic_engine.cpp
    src/diagnostic_formatter.cpp
    src/diagnostic_json.cpp
    src/diagnostic_reporter.cpp
    src/error_code.cpp
)

target_include_directories(myc_diagnostics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_diagnostics
    PUBLIC
        myc_shared
    PRIVATE
        myc_source
)

myc_set_target_standard(myc_diagnostics)
myc_set_warnings(myc_diagnostics)

add_library(myc::diagnostics ALIAS myc_diagnostics)
