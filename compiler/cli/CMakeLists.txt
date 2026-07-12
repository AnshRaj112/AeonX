add_library(myc_cli STATIC
    src/cli_parser.cpp
)

target_include_directories(myc_cli
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_cli
    PUBLIC
        myc_common
        myc_exitcodes
)

myc_set_target_standard(myc_cli)
myc_set_warnings(myc_cli)

add_library(myc::cli ALIAS myc_cli)
