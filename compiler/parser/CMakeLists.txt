add_library(myc_parser STATIC src/parser_module.cpp)

target_include_directories(myc_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(myc_parser PUBLIC myc_lexer myc_ast)

myc_set_target_standard(myc_parser)
myc_set_warnings(myc_parser)
add_library(myc::parser ALIAS myc_parser)
