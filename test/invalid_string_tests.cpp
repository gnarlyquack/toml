#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_multiline_bad_escape_3)
{
    const string toml{"# \\<Space> is not a valid escape.\nk = \"\"\"t\\ \"\"\"\n\n", 49};

    const vector<Error> expected{{ 2, 11, "Invalid escape: ' '." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_multiline_no_close)
{
    const string toml{"invalid = \"\"\"\n    this will fail\n", 33};

    const vector<Error> expected{{ 3, 1, "Unterminated string." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_multiline_bad_escape_1)
{
    const string toml{"k = \"\"\"t\\a\"\"\"\n\n", 15};

    const vector<Error> expected{{ 1, 10, "Invalid escape: 'a'." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_no_close)
{
    const string toml{"no-ending-quote = \"One time, at band camp\n", 42};

    const vector<Error> expected{{ 1, 42, "Unterminated string." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_unknown_escape)
{
    const string toml{"a = \"\\@\"\n", 9};

    const vector<Error> expected{{ 1, 7, "Invalid escape: '@'." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_concat)
{
    const string toml{"no_concat = \"first\" \"second\"\n", 29};

    const vector<Error> expected{{ 1, 21, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_codepoint)
{
    const string toml{"invalid-codepoint = \"This string contains a non scalar unicode codepoint \\uD801\"\n", 81};

    const vector<Error> expected{{ 1, 1, "Invalid Unicode character U+D801." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_uni_esc_2)
{
    const string toml{"str = \"val\\Ux\"\n", 15};

    const vector<Error> expected{{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_byte_escape)
{
    const string toml{"naughty = \"\\xAg\"\n", 17};

    const vector<Error> expected{{ 1, 13, "Invalid escape: 'x'." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_uni_esc_5)
{
    const string toml{"str = \"val\\Ugggggggg\"\n", 22};

    const vector<Error> expected{{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_uni_esc_4)
{
    const string toml{"str = \"val\\U0000\"\n", 18};

    const vector<Error> expected{{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_multiline_unknown_escape)
{
    const string toml{"a = \"\"\"\\@\"\"\"\n", 13};

    const vector<Error> expected{{ 1, 9, "Invalid escape: '@'." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_literal_multiline_quotes_2)
{
    const string toml{"a = '''15 apostrophes: ''''''''''''''''''\n", 42};

    const vector<Error> expected{{ 1, 29, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_escape_2)
{
    const string toml{"invalid-escape = \"This string has a bad \\  escape character.\"\n\n", 63};

    const vector<Error> expected{{ 1, 42, "Invalid escape: ' '." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_byte_escapes)
{
    const string toml{"answer = \"\\x33\"\n", 16};

    const vector<Error> expected{{ 1, 12, "Invalid escape: 'x'." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_wrong_close)
{
    const string toml{"bad-ending-quote = \"double and single'\n", 39};

    const vector<Error> expected{{ 1, 39, "Unterminated string." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_uni_esc_3)
{
    const string toml{"str = \"val\\U0000000\"\n\n", 22};

    const vector<Error> expected{{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_multiline)
{
    const string toml{"multi = \"first line\nsecond line\"\n", 33};

    const vector<Error> expected{{ 1, 20, "Unterminated string." },{ 2, 8, "Missing '=' between key and value." },{ 2, 8, "Invalid value: line\"." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_multiline_quotes)
{
    const string toml{"str5 = \"\"\"Here are three quotation marks: \"\"\".\"\"\"\n", 50};

    const vector<Error> expected{{ 1, 46, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_out_of_range_unicode_escape_2)
{
    const string toml{"a = \"\\U00D80000\"\n", 17};

    const vector<Error> expected{{ 1, 0, "Invalid Unicode character U+D80000." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_multiline_bad_escape_2)
{
    const string toml{"# \\<Space> is not a valid escape.\nk = \"\"\"t\\ t\"\"\"\n", 49};

    const vector<Error> expected{{ 2, 11, "Invalid escape: ' '." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_missing_quotes)
{
    const string toml{"name = value\n", 13};

    const vector<Error> expected{{ 1, 8, "Invalid value: value." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_slash_escape)
{
    const string toml{"invalid-escape = \"This string has a bad \\/ escape character.\"\n", 62};

    const vector<Error> expected{{ 1, 42, "Invalid escape: '/'." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_escape_1)
{
    const string toml{"invalid-escape = \"This string has a bad \\a escape character.\"\n", 62};

    const vector<Error> expected{{ 1, 42, "Invalid escape: 'a'." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_multiline_quotes_1)
{
    const string toml{"a = \"\"\"6 quotes: \"\"\"\"\"\"\n", 24};

    const vector<Error> expected{{ 1, 23, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_bad_uni_esc_1)
{
    const string toml{"str = \"val\\ue\"\n", 15};

    const vector<Error> expected{{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },{ 1, 13, "Unicode escape sequence contains non-hexadecimal value." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_text_after_string)
{
    const string toml{"string = \"Is there life after strings?\" No.\n", 44};

    const vector<Error> expected{{ 1, 41, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_multiline_out_of_range_unicode_escape_2)
{
    const string toml{"a = \"\"\"\\U00D80000\"\"\"\n", 21};

    const vector<Error> expected{{ 1, 0, "Invalid Unicode character U+D80000." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_literal_multiline_quotes_1)
{
    const string toml{"a = '''6 apostrophes: ''''''\n\n", 30};

    const vector<Error> expected{{ 1, 28, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_multiline_no_close_2)
{
    const string toml{"x=\"\"\"\n", 6};

    const vector<Error> expected{{ 2, 1, "Unterminated string." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_out_of_range_unicode_escape_1)
{
    const string toml{"a = \"\\UFFFFFFFF\"\n", 17};

    const vector<Error> expected{{ 1, 6, "Invalid Unicode character U+FFFFFFFF." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_basic_multiline_out_of_range_unicode_escape_1)
{
    const string toml{"a = \"\"\"\\UFFFFFFFF\"\"\"\n", 21};

    const vector<Error> expected{{ 1, 8, "Invalid Unicode character U+FFFFFFFF." },};

    assert_errors(toml, expected);
}


TEST(invalid_string_tests, test_multiline_escape_space)
{
    const string toml{"a = \"\"\"\n  foo \\ \\n\n  bar\"\"\"\n", 28};

    const vector<Error> expected{{ 2, 9, "Invalid escape: ' '." },};

    assert_errors(toml, expected);
}
