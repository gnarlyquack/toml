#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_array_tests, test_no_close)
{
    const string toml{"long_array = [ 1, 2, 3\n", 23};

    const vector<Error> expected{{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_tables_2)
{
    const string toml{"# INVALID TOML DOC\n[[fruit]]\n  name = \"apple\"\n\n  [[fruit.variety]]\n    name = \"red delicious\"\n\n  # This table conflicts with the previous table\n  [fruit.variety]\n    name = \"granny smith\"\n", 188};

    const vector<Error> expected{{ 9, 10, "Cannot define 'variety' as a table because it has already been defined as an array on line 5, character 11." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_double_comma_1)
{
    const string toml{"array = [1,,2]\n", 15};

    const vector<Error> expected{{ 1, 12, "Missing value for array." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_missing_separator)
{
    const string toml{"wrong = [ 1 2 3 ]\n", 18};

    const vector<Error> expected{{ 1, 13, "Missing ',' between array values." },{ 1, 15, "Missing ',' between array values." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_double_comma_2)
{
    const string toml{"array = [1,2,,]\n\n", 17};

    const vector<Error> expected{{ 1, 14, "Missing value for array." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_text_in_array)
{
    const string toml{"array = [\n  \"Entry 1\",\n  I don't belong,\n  \"Entry 2\",\n]\n", 56};

    const vector<Error> expected{{ 3, 3, "Invalid value: I don't belong." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_text_before_array_separator)
{
    const string toml{"array = [\n  \"Is there life before an array separator?\" No,\n  \"Entry\"\n]\n", 71};

    const vector<Error> expected{{ 2, 46, "Missing ',' between array values." },{ 2, 46, "Invalid value: No." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_tables_1)
{
    const string toml{"# INVALID TOML DOC\nfruit = []\n\n[[fruit]] # Not allowed\n", 55};

    const vector<Error> expected{{ 4, 3, "Key 'fruit' has already been defined on line 2, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_text_after_array_entries)
{
    const string toml{"array = [\n  \"Is there life after an array separator?\", No\n  \"Entry\"\n]\n", 70};

    const vector<Error> expected{{ 2, 46, "Invalid value: No." },{ 3, 3, "Missing ',' between array values." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_no_close_table_2)
{
    const string toml{"x = [{ key = 42 #\n", 18};

    const vector<Error> expected{{ 1, 17, "Missing ',' between inline table values." },{ 1, 17, "Invalid key: #." },{ 1, 18, "Missing '=' between key and value." },{ 1, 18, "Missing value." },{ 1, 18, "Unterminated inline table." },{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_no_close_table)
{
    const string toml{"x = [{ key = 42\n", 16};

    const vector<Error> expected{{ 1, 16, "Unterminated inline table." },{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_extending_table)
{
    const string toml{"a = [{ b = 1 }]\n\n# Cannot extend tables within static arrays\n# https://github.com/toml-lang/toml/issues/908\n[a.c]\nfoo = 1\n", 122};

    const vector<Error> expected{{ 5, 2, "Cannot add keys to 'a' because it has been defined as an array on line 1, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_array_tests, test_no_close_2)
{
    const string toml{"x = [42 #\n", 10};

    const vector<Error> expected{{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}
