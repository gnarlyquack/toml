#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_inline_table_tests, test_empty)
{
    const string toml{"t = {,}\n", 8};

    const vector<Error> expected{{ 1, 6, "Missing value for inline table." },{ 1, 8, "Trailing ',' not allowed in inline table." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_no_comma)
{
    const string toml{"t = {x = 3 y = 4}\n", 18};

    const vector<Error> expected{{ 1, 12, "Missing ',' between inline table values." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_trailing_comma)
{
    const string toml{"# A terminating comma (also called trailing comma) is not permitted after the\n# last key/value pair in an inline table\nabc = { abc = 123, }\n", 140};

    const vector<Error> expected{{ 3, 21, "Trailing ',' not allowed in inline table." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_duplicate_key)
{
    const string toml{"# Duplicate keys within an inline table are invalid\na={b=1, b=2}\n", 65};

    const vector<Error> expected{{ 2, 9, "Key 'b' has already been defined on line 2, character 4." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_double_comma)
{
    const string toml{"t = {x=3,,y=4}\n", 15};

    const vector<Error> expected{{ 1, 10, "Missing value for inline table." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_linebreak_4)
{
    const string toml{"json_like = {\n          first = \"Tom\",\n          last = \"Preston-Werner\"\n}\n", 75};

    const vector<Error> expected{{ 1, 14, "Unterminated inline table." },{ 2, 24, "Expected a new line after expression." },{ 4, 1, "Invalid key: }." },{ 4, 2, "Missing '=' between key and value." },{ 4, 2, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_linebreak_2)
{
    const string toml{"t = {a=1,\nb=2}\n", 15};

    const vector<Error> expected{{ 1, 10, "Unterminated inline table." },{ 2, 3, "Invalid value for decimal integer: 2}." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_linebreak_3)
{
    const string toml{"t = {a=1\n,b=2}\n", 15};

    const vector<Error> expected{{ 1, 9, "Unterminated inline table." },{ 2, 1, "Invalid key: ,b." },{ 2, 4, "Invalid value for decimal integer: 2}." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_overwrite)
{
    const string toml{"a.b=0\n# Since table \"a\" is already defined, it can't be replaced by an inline table.\na={}\n", 90};

    const vector<Error> expected{{ 3, 1, "Key 'a' has already been defined on line 1, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_add)
{
    const string toml{"a={}\n# Inline tables are immutable and can't be extended\n[a.b]\n", 63};

    const vector<Error> expected{{ 3, 2, "Cannot add keys to 'a' because it has been defined as an inline table on line 1, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_inline_table_tests, test_linebreak_1)
{
    const string toml{"# No newlines are allowed between the curly braces unless they are valid within\n# a value.\nsimple = { a = 1 \n}\n", 111};

    const vector<Error> expected{{ 3, 18, "Unterminated inline table." },{ 4, 1, "Invalid key: }." },{ 4, 2, "Missing '=' between key and value." },{ 4, 2, "Missing value." },};

    assert_errors(toml, expected);
}
