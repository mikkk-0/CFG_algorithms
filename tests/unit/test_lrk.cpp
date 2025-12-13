#include <gtest/gtest.h>
#include "lrk.hpp"

TEST(GrammarTest, CorrectTest) {
    std::string gram_path = "c_grammar.txt";
    std::string test_path = "c_tests.txt";
    Analyzer* an = new LangRecogn;
    an->init(read_grammar(gram_path));
    auto tests = read_tests(test_path);
    ASSERT_EQ(an->proceed(tests[0]), true);
    ASSERT_EQ(an->proceed(tests[1]), false);
    ASSERT_EQ(an->proceed(tests[2]), false);
}

TEST(GrammarTest, RuleExceptions) {
    for (size_t i = 1; i <= 3; ++i) {
        std::string gram_path = "i" + std::to_string(i) + "_grammar.txt";
        ASSERT_THROW(read_grammar(gram_path), RuleException);
    }
}