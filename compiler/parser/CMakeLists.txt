add_library(myc_parser STATIC
    src/parser_module.cpp
    src/parser.cpp
    src/parser_context.cpp
    src/parser_statistics.cpp
    src/token_cursor.cpp
    src/recovery_manager.cpp
    src/literal_parser.cpp
    src/type_parser.cpp
    src/expression_parser.cpp
    src/statement_parser.cpp
    src/declaration_parser.cpp
    src/import_parser.cpp
    src/attribute_parser.cpp
    src/future_pattern_parser.cpp
    src/future_macro_parser.cpp
    src/future_generic_parser.cpp
)

target_include_directories(myc_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(myc_parser PUBLIC myc_lexer myc_ast myc_diagnostics)

myc_set_target_standard(myc_parser)
myc_set_warnings(myc_parser)
add_library(myc::parser ALIAS myc_parser)
