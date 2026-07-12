add_library(myc_lexer STATIC src/lexer_module.cpp)

target_include_directories(myc_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(myc_lexer PUBLIC myc_shared myc_source)

myc_set_target_standard(myc_lexer)
myc_set_warnings(myc_lexer)
add_library(myc::lexer ALIAS myc_lexer)
