add_executable(myc_tests
    myc_test_main.cpp
    test_character_stream.cpp
    test_config.cpp
    test_diagnostics.cpp
    test_lexer.cpp
    test_lexer_infrastructure.cpp
    test_lexer_production.cpp
    test_lexer_debug.cpp
    test_source.cpp
    test_version.cpp
    test_cli.cpp
)

target_include_directories(myc_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(myc_tests PRIVATE myc_driver myc_commands myc_cli myc_lexer myc_shared)

myc_set_target_standard(myc_tests)
myc_set_warnings(myc_tests)

add_test(NAME myc_unit_tests COMMAND myc_tests)
