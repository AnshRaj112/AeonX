add_library(myc_config STATIC
    src/compiler_config.cpp
)

target_include_directories(myc_config
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_config PUBLIC myc_shared)

myc_set_target_standard(myc_config)
myc_set_warnings(myc_config)

add_library(myc::config ALIAS myc_config)
