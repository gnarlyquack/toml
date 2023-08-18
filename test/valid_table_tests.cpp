#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_empty)
{
    const string toml = "[a]\n";

    const Table expected{{ "a", new TableValue({}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_without_super)
{
    const string toml = "# [x] you\n# [x.y] don't\n# [x.y.z] need these\n[x.y.z.w] # for this to work\n[x] # defining a super-table afterwards is ok\n";

    const Table expected{{ "x", new TableValue({{ "y", new TableValue({{ "z", new TableValue({{ "w", new TableValue({}) },}) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_array_many)
{
    const string toml = "[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n\n[[people]]\nfirst_name = \"Eric\"\nlast_name = \"Clapton\"\n\n[[people]]\nfirst_name = \"Bob\"\nlast_name = \"Seger\"\n";

    const Table expected{{ "people", new ArrayValue({new TableValue({{ "first_name", new StringValue("Bruce") },{ "last_name", new StringValue("Springsteen") },}),new TableValue({{ "first_name", new StringValue("Eric") },{ "last_name", new StringValue("Clapton") },}),new TableValue({{ "first_name", new StringValue("Bob") },{ "last_name", new StringValue("Seger") },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_no_eol)
{
    const string toml = "[table]\n";

    const Table expected{{ "table", new TableValue({}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_names)
{
    const string toml = "[a.b.c]\n[a.\"b.c\"]\n[a.'d.e']\n[a.' x ']\n[ d.e.f ]\n[ g . h . i ]\n[ j . \"\312\236\" . 'l' ]\n\n[x.1.2]\n";

    const Table expected{{ "a", new TableValue({{ " x ", new TableValue({}) },{ "b", new TableValue({{ "c", new TableValue({}) },}) },{ "b.c", new TableValue({}) },{ "d.e", new TableValue({}) },}) },{ "d", new TableValue({{ "e", new TableValue({{ "f", new TableValue({}) },}) },}) },{ "g", new TableValue({{ "h", new TableValue({{ "i", new TableValue({}) },}) },}) },{ "j", new TableValue({{ "ʞ", new TableValue({{ "l", new TableValue({}) },}) },}) },{ "x", new TableValue({{ "1", new TableValue({{ "2", new TableValue({}) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_with_literal_string)
{
    const string toml = "['a']\n[a.'\"b\"']\n[a.'\"b\"'.c]\nanswer = 42 \n";

    const Table expected{{ "a", new TableValue({{ "\"b\"", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_whitespace)
{
    const string toml = "[\"valid key\"]\n";

    const Table expected{{ "valid key", new TableValue({}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_with_single_quotes)
{
    const string toml = "['a']\n[a.'b']\n[a.'b'.c]\nanswer = 42 \n";

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_keyword)
{
    const string toml = "[true]\n\n[false]\n\n[inf]\n\n[nan]\n\n\n";

    const Table expected{{ "true", new TableValue({}) },{ "false", new TableValue({}) },{ "inf", new TableValue({}) },{ "nan", new TableValue({}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_array_table_array)
{
    const string toml = "[[a]]\n    [[a.b]]\n        [a.b.c]\n            d = \"val0\"\n    [[a.b]]\n        [a.b.c]\n            d = \"val1\"\n";

    const Table expected{{ "a", new ArrayValue({new TableValue({{ "b", new ArrayValue({new TableValue({{ "c", new TableValue({{ "d", new StringValue("val0") },}) },}),new TableValue({{ "c", new TableValue({{ "d", new StringValue("val1") },}) },}),}) },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_array_one)
{
    const string toml = "[[people]]\nfirst_name = \"Bruce\"\nlast_name = \"Springsteen\"\n";

    const Table expected{{ "people", new ArrayValue({new TableValue({{ "first_name", new StringValue("Bruce") },{ "last_name", new StringValue("Springsteen") },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_array_nest)
{
    const string toml = "[[albums]]\nname = \"Born to Run\"\n\n  [[albums.songs]]\n  name = \"Jungleland\"\n\n  [[albums.songs]]\n  name = \"Meeting Across the River\"\n\n[[albums]]\nname = \"Born in the USA\"\n  \n  [[albums.songs]]\n  name = \"Glory Days\"\n\n  [[albums.songs]]\n  name = \"Dancing in the Dark\"\n";

    const Table expected{{ "albums", new ArrayValue({new TableValue({{ "name", new StringValue("Born to Run") },{ "songs", new ArrayValue({new TableValue({{ "name", new StringValue("Jungleland") },}),new TableValue({{ "name", new StringValue("Meeting Across the River") },}),}) },}),new TableValue({{ "name", new StringValue("Born in the USA") },{ "songs", new ArrayValue({new TableValue({{ "name", new StringValue("Glory Days") },}),new TableValue({{ "name", new StringValue("Dancing in the Dark") },}),}) },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_sub_empty)
{
    const string toml = "[a]\n[a.b]\n";

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_array_implicit)
{
    const string toml = "[[albums.songs]]\nname = \"Glory Days\"\n";

    const Table expected{{ "albums", new TableValue({{ "songs", new ArrayValue({new TableValue({{ "name", new StringValue("Glory Days") },}),}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_table_tests, test_with_pound)
{
    const string toml = "[\"key#group\"]\nanswer = 42\n";

    const Table expected{{ "key#group", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },};

    assert_parsed(toml, expected);
}
