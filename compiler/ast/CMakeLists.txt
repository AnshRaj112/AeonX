add_library(myc_ast STATIC src/ast_module.cpp)

target_include_directories(myc_ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(myc_ast PUBLIC myc_shared)

myc_set_target_standard(myc_ast)
myc_set_warnings(myc_ast)
add_library(myc::ast ALIAS myc_ast)
