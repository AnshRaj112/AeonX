add_library(myc_driver STATIC
    src/cli_parser.cpp
    src/command_dispatcher.cpp
    src/compiler_driver.cpp
)

target_include_directories(myc_driver
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_driver
    PUBLIC
        myc_config
        myc_diagnostics
        myc_source
        myc_shared
)

myc_set_target_standard(myc_driver)
myc_set_warnings(myc_driver)

add_library(myc::driver ALIAS myc_driver)
