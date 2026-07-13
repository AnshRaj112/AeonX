add_library(myc_commands STATIC
    src/builtin_commands.cpp
    src/command_registry.cpp
    src/help_command.cpp
    src/placeholder_command.cpp
    src/tokens_command.cpp
    src/version_command.cpp
)

target_include_directories(myc_commands
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(myc_commands
    PUBLIC
        myc_cli
        myc_common
        myc_config
        myc_diagnostics
        myc_exitcodes
        myc_lexer
        myc_source
        myc_shared
)

myc_set_target_standard(myc_commands)
myc_set_warnings(myc_commands)

add_library(myc::commands ALIAS myc_commands)
