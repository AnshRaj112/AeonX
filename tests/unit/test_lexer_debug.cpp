#include "myc_test.hpp"

#include "myc/lexer/keyword.hpp"
#include "myc/lexer/operator.hpp"

MYC_TEST(lexer_prod_keyword_string_table) {
    const myc::lexer::Keyword* keyword = myc::lexer::KeywordTable::Lookup("string");
    MYC_ASSERT(keyword != nullptr);
    MYC_ASSERT(keyword->type == myc::lexer::TokenType::TyString);
}

MYC_TEST(lexer_prod_operator_table_specials) {
    MYC_ASSERT(myc::lexer::OperatorTable::Lookup("@") != nullptr);
    MYC_ASSERT(myc::lexer::OperatorTable::Lookup("#") != nullptr);
    MYC_ASSERT(myc::lexer::OperatorTable::Lookup("$") != nullptr);
    MYC_ASSERT(myc::lexer::OperatorTable::Lookup("^") != nullptr);
    MYC_ASSERT(myc::lexer::OperatorTable::Lookup("+") != nullptr);
}
