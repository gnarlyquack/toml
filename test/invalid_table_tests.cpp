#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_table_tests, test_empty)
{
    const string toml{"[]\n", 3};

    const vector<Error> expected{{ 1, 2, "Missing key." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_redefine)
{
    const string toml{"# Define b as int, and try to use it as a table: error\n[a]\nb = 1\n\n[a.b]\nc = 2\n", 78};

    const vector<Error> expected{{ 5, 4, "Key 'b' has already been defined on line 3, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_text_after_table)
{
    const string toml{"[error] this shouldn't be here\n", 31};

    const vector<Error> expected{{ 1, 9, "Expected the end of the line but got: this shouldn't be here" },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_array_empty)
{
    const string toml{"[[]]\nname = \"Born to Run\"\n", 26};

    const vector<Error> expected{{ 1, 3, "Missing key." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_nested_brackets_open)
{
    const string toml{"[a[b]\nzyx = 42\n", 15};

    const vector<Error> expected{{ 1, 2, "Invalid key: a[b" },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_array_missing_bracket)
{
    const string toml{"[[albums]\nname = \"Born to Run\"\n", 31};

    const vector<Error> expected{{ 1, 9, "Missing closing ']' for array of tables." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_llbrace)
{
    const string toml{"[ [table]]\n", 11};

    const vector<Error> expected{{ 1, 3, "Invalid key: [table" },{ 1, 10, "Expected the end of the line but got: ]" },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_empty_implicit_table)
{
    const string toml{"[naughty..naughty]\n", 19};

    const vector<Error> expected{{ 1, 10, "Missing key." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_rrbrace)
{
    const string toml{"[[table] ]\n", 11};

    const vector<Error> expected{{ 1, 8, "Missing closing ']' for array of tables." },{ 1, 10, "Expected the end of the line but got: ]" },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_duplicate_key_dotted_table2)
{
    const string toml{"[fruit]\napple.taste.sweet = true\n\n[fruit.apple.taste] # INVALID\n", 64};

    const vector<Error> expected{{ 4, 14, "Key 'taste' has already been defined on line 2, character 7." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_append_with_dotted_keys_1)
{
    const string toml{"# First a.b.c defines a table: a.b.c = {z=9}\n#\n# Then we define a.b.c.t = \"str\" to add a str to the above table, making it:\n#\n#   a.b.c = {z=9, t=\"...\"}\n#\n# While this makes sense, logically, it was decided this is not valid TOML as\n# it's too confusing/convoluted.\n# \n# See: https://github.com/toml-lang/toml/issues/846\n#      https://github.com/toml-lang/toml/pull/859\n\n[a.b.c]\n  z = 9\n\n[a]\n  b.c.t = \"Using dotted keys to add to [a.b.c] after explicitly defining it above is not allowed\"\n", 491};

    const vector<Error> expected{{ 17, 5, "Key 'c' has already been defined on line 13, character 6." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_whitespace)
{
    const string toml{"[invalid key]\n", 14};

    const vector<Error> expected{{ 1, 2, "Invalid key: invalid key" },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_duplicate_table_array2)
{
    const string toml{"[[tbl]]\n[tbl]\n", 14};

    const vector<Error> expected{{ 2, 2, "Cannot define 'tbl' as a table because it has already been defined as an array on line 1, character 3." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_duplicate_key_dotted_table)
{
    const string toml{"[fruit]\napple.color = \"red\"\n\n[fruit.apple] # INVALID\n", 53};

    const vector<Error> expected{{ 4, 8, "Key 'apple' has already been defined on line 2, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_append_with_dotted_keys_2)
{
    const string toml{"# This is the same issue as in injection-1.toml, except that nests one level\n# deeper. See that file for a more complete description.\n\n[a.b.c.d]\n  z = 9\n\n[a]\n  b.c.d.k.t = \"Using dotted keys to add to [a.b.c.d] after explicitly defining it above is not allowed\"\n", 262};

    const vector<Error> expected{{ 8, 7, "Key 'd' has already been defined on line 4, character 8." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_equals_sign)
{
    const string toml{"[name=bad]\n", 11};

    const vector<Error> expected{{ 1, 2, "Invalid key: name=bad" },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_quoted_no_close)
{
    const string toml{"[\"where will it end]\nname = value\n", 34};

    const vector<Error> expected{{ 1, 21, "Unterminated string." },{ 1, 21, "Missing closing ']' for table." },{ 2, 8, "Invalid value: value." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_duplicate_key_table)
{
    const string toml{"[fruit]\ntype = \"apple\"\n\n[fruit.type]\napple = \"yes\"\n", 51};

    const vector<Error> expected{{ 4, 8, "Key 'type' has already been defined on line 2, character 1." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_duplicate)
{
    const string toml{"[a]\nb = 1\n\n[a]\nc = 2\n", 21};

    const vector<Error> expected{{ 4, 2, "Key 'a' has already been defined on line 1, character 2." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_nested_brackets_close)
{
    const string toml{"[a]b]\nzyx = 42\n", 15};

    const vector<Error> expected{{ 1, 4, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_duplicate_table_array)
{
    const string toml{"[tbl]\n[[tbl]]\n", 14};

    const vector<Error> expected{{ 2, 3, "Key 'tbl' has already been defined on line 1, character 2." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_array_implicit)
{
    const string toml{"# This test is a bit tricky. It should fail because the first use of\n# `[[albums.songs]]` without first declaring `albums` implies that `albums`\n# must be a table. The alternative would be quite weird. Namely, it wouldn't\n# comply with the TOML spec: \"Each double-bracketed sub-table will belong to \n# the most *recently* defined table element *above* it.\"\n#\n# This is in contrast to the *valid* test, table-array-implicit where\n# `[[albums.songs]]` works by itself, so long as `[[albums]]` isn't declared\n# later. (Although, `[albums]` could be.)\n[[albums.songs]]\nname = \"Glory Days\"\n\n[[albums]]\nname = \"Born in the USA\"\n", 622};

    const vector<Error> expected{{ 13, 3, "Cannot define 'albums' as an array because it has already been defined as a table on line 10, character 3." },};

    assert_errors(toml, expected);
}


TEST(invalid_table_tests, test_with_pound)
{
    const string toml{"[key#group]\nanswer = 42\n", 24};

    const vector<Error> expected{{ 1, 2, "Invalid key: key#group." },};

    assert_errors(toml, expected);
}
