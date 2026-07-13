add_library(myc_lexer STATIC
    src/character_reader.cpp
    src/character_stream.cpp
    src/keyword.cpp
    src/lexer.cpp
    src/lexer_configuration.cpp
    src/lexer_context.cpp
    src/lexer_module.cpp
    src/lexer_statistics.cpp
    src/operator.cpp
    src/scanner_registry.cpp
    src/scanners/annotation_scanner.cpp
    src/scanners/character_scanner.cpp
    src/scanners/comment_scanner.cpp
    src/scanners/directive_scanner.cpp
    src/scanners/identifier_scanner.cpp
    src/scanners/keyword_scanner.cpp
    src/scanners/number_scanner.cpp
    src/scanners/operator_scanner.cpp
    src/scanners/scanner_utils.cpp
    src/scanners/string_scanner.cpp
    src/scanners/unknown_scanner.cpp
    src/scanners/whitespace_scanner.cpp
    src/token_category_map.cpp
    src/token_factory.cpp
    src/token_printer.cpp
    src/token_stream.cpp
    src/token_type.cpp
)

target_include_directories(myc_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(myc_lexer PUBLIC myc_shared myc_source myc_diagnostics)

myc_set_target_standard(myc_lexer)
myc_set_warnings(myc_lexer)
add_library(myc::lexer ALIAS myc_lexer)
