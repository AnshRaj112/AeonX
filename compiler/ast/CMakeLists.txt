add_library(myc_ast STATIC
    src/ast_module.cpp
    src/ast_node.cpp
    src/ast_accept.cpp
    src/ast_visitor.cpp
    src/ast_pretty_printer.cpp
)

target_include_directories(myc_ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(myc_ast PUBLIC myc_shared myc_lexer)

myc_set_target_standard(myc_ast)
myc_set_warnings(myc_ast)
add_library(myc::ast ALIAS myc_ast)
