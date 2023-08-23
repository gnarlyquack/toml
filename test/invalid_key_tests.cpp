#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_empty)
{
    const string toml{" = 1\n", 5};

    const vector<Error> expected{{ 1, 2, "Missing key." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_quoted_unclosed_2)
{
    const string toml{"\"key\n", 5};

    const vector<Error> expected{{ 1, 5, "Unterminated string." },{ 1, 5, "Missing '=' between key and value." },{ 1, 5, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_after_value)
{
    const string toml{"first = \"Tom\" last = \"Preston-Werner\" # INVALID\n", 48};

    const vector<Error> expected{{ 1, 15, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_single_open_bracket)
{
    const string toml{"[\n", 2};

    const vector<Error> expected{{ 1, 2, "Missing key." },{ 1, 2, "Missing closing ']' for table." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_special_character)
{
    const string toml{"\316\274 = \"greek small letter mu\"\n", 29};

    const vector<Error> expected{{ 1, 1, "Invalid key: μ." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_without_value_1)
{
    const string toml{"key\n", 4};

    const vector<Error> expected{{ 1, 4, "Missing '=' between key and value." },{ 1, 4, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_without_value_3)
{
    const string toml{"\"key\"\n", 6};

    const vector<Error> expected{{ 1, 6, "Missing '=' between key and value." },{ 1, 6, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_multiline)
{
    const string toml{"\"\"\"long\nkey\"\"\" = 1\n", 19};

    const vector<Error> expected{{ 1, 1, "Invalid key: \"\"\"long." },{ 1, 8, "Missing '=' between key and value." },{ 1, 8, "Missing value." },{ 2, 1, "Invalid key: key\"\"\"." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_open_bracket)
{
    const string toml{"[abc = 1\n", 9};

    const vector<Error> expected{{ 1, 2, "Invalid key: abc = 1." },{ 1, 9, "Missing closing ']' for table." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_start_bracket)
{
    const string toml{"[a]\n[xyz = 5\n[b]\n", 17};

    const vector<Error> expected{{ 2, 2, "Invalid key: xyz = 5." },{ 2, 9, "Missing closing ']' for table." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_quoted_unclosed_1)
{
    const string toml{"\"key = x\n", 9};

    const vector<Error> expected{{ 1, 9, "Unterminated string." },{ 1, 9, "Missing '=' between key and value." },{ 1, 9, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_start_dot)
{
    const string toml{".key = 1\n", 9};

    const vector<Error> expected{{ 1, 1, "Missing key." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_without_value_4)
{
    const string toml{"\"key\" = \n", 9};

    const vector<Error> expected{{ 1, 9, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_duplicate_keys)
{
    const string toml{"dupe = false\ndupe = true\n", 25};

    const vector<Error> expected{{ 2, 1, "Key 'dupe' has already been defined on line 1, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_after_table)
{
    const string toml{"[error] this = \"should not be here\"\n", 36};

    const vector<Error> expected{{ 1, 9, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_no_eol)
{
    const string toml{"a = 1 b = 2\n", 12};

    const vector<Error> expected{{ 1, 7, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_two_equals)
{
    const string toml{"key= = 1\n", 9};

    const vector<Error> expected{{ 1, 6, "Invalid value: = 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_without_value_2)
{
    const string toml{"key = \n", 7};

    const vector<Error> expected{{ 1, 7, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_partial_quoted)
{
    const string toml{"partial\"quoted\" = 5\n", 20};

    const vector<Error> expected{{ 1, 1, "Invalid key: partial\"quoted\"." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_two_equals3)
{
    const string toml{"a=b=1\n", 6};

    const vector<Error> expected{{ 1, 3, "Invalid value: b=1." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_newline)
{
    const string toml{"barekey\n   = 123\n", 17};

    const vector<Error> expected{{ 1, 8, "Missing '=' between key and value." },{ 1, 8, "Missing value." },{ 2, 4, "Missing key." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_space)
{
    const string toml{"a b = 1\n", 8};

    const vector<Error> expected{{ 1, 1, "Invalid key: a b." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_after_array)
{
    const string toml{"[[agencies]] owner = \"S Cjelli\"\n", 32};

    const vector<Error> expected{{ 1, 14, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_escape)
{
    const string toml{"\\u00c0 = \"latin capital letter A with grave\"\n", 45};

    const vector<Error> expected{{ 1, 1, "Invalid key: \\u00c0." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_hash)
{
    const string toml{"a# = 1\n", 7};

    const vector<Error> expected{{ 1, 2, "Missing '=' between key and value." },{ 1, 2, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_bare_invalid_character)
{
    const string toml{"bare!key = 123\n", 15};

    const vector<Error> expected{{ 1, 1, "Invalid key: bare!key." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_two_equals2)
{
    const string toml{"a==1\n", 5};

    const vector<Error> expected{{ 1, 3, "Invalid value: =1." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_duplicate)
{
    const string toml{"# DO NOT DO THIS\nname = \"Tom\"\nname = \"Pradyun\"\n", 47};

    const vector<Error> expected{{ 3, 1, "Key 'name' has already been defined on line 2, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_key_tests, test_dotted_redefine_table)
{
    const string toml{"# Defined a.b as int\na.b = 1\n# Tries to access it as table: error\na.b.c = 2\n", 76};

    const vector<Error> expected{{ 4, 3, "Cannot add keys to 'b' because it has been defined as an integer on line 2, character 3." },};

    assert_errors(toml, expected);
}
