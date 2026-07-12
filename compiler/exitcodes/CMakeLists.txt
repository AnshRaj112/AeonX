add_library(myc_exitcodes INTERFACE)

target_include_directories(myc_exitcodes
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(myc::exitcodes ALIAS myc_exitcodes)
