add_library(myc_common STATIC
    src/cli_output.cpp
)

target_include_directories(myc_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_common
    PUBLIC
        myc_config
        myc_shared
)

myc_set_target_standard(myc_common)
myc_set_warnings(myc_common)

add_library(myc::common ALIAS myc_common)
