#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_empty)
{
    const string toml{"thevoid = [[[[[]]]]]\n", 21};

    const Table expected{{ "thevoid", new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({}),}),}),}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_mixed_int_array)
{
    const string toml{"arrays-and-ints =  [1, [\"Arrays are not integers.\"]]\n", 53};

    const Table expected{{ "arrays-and-ints", new ArrayValue({new IntegerValue(1ULL),new ArrayValue({new StringValue("Arrays are not integers."),}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_mixed_string_table)
{
    const string toml{"contributors = [\n  \"Foo Bar <foo@example.com>\",\n  { name = \"Baz Qux\", email = \"bazqux@example.com\", url = \"https://example.com/bazqux\" }\n]\n\n# Start with a table as the first element. This tests a case that some libraries\n# might have where they will check if the first entry is a table/map/hash/assoc\n# array and then encode it as a table array. This was a reasonable thing to do\n# before TOML 1.0 since arrays could only contain one type, but now it's no\n# longer.\nmixed = [{k=\"a\"}, \"b\", 1]\n", 492};

    const Table expected{{ "contributors", new ArrayValue({new StringValue("Foo Bar <foo@example.com>"),new TableValue({{ "email", new StringValue("bazqux@example.com") },{ "name", new StringValue("Baz Qux") },{ "url", new StringValue("https://example.com/bazqux") },}),}) },{ "mixed", new ArrayValue({new TableValue({{ "k", new StringValue("a") },}),new StringValue("b"),new IntegerValue(1ULL),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_nospaces)
{
    const string toml{"ints = [1,2,3]\n", 15};

    const Table expected{{ "ints", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new IntegerValue(3ULL),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_table_array_string_backslash)
{
    const string toml{"foo = [ { bar=\"\\\"{{baz}}\\\"\"} ]\n", 31};

    const Table expected{{ "foo", new ArrayValue({new TableValue({{ "bar", new StringValue("\"{{baz}}\"") },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_nested_inline_table)
{
    const string toml{"a = [ { b = {} } ]\n", 19};

    const Table expected{{ "a", new ArrayValue({new TableValue({{ "b", new TableValue({}) },}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_array)
{
    const string toml{"ints = [1, 2, 3, ]\nfloats = [1.1, 2.1, 3.1]\nstrings = [\"a\", \"b\", \"c\"]\ndates = [\n  1987-07-05T17:45:00Z,\n  1979-05-27T07:32:00Z,\n  2006-06-01T11:00:00Z,\n]\ncomments = [\n         1,\n         2, #this is ok\n]\n", 205};

    const Table expected{{ "comments", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),}) },{ "dates", new ArrayValue({new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)),new OffsetDateTimeValue(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)),new OffsetDateTimeValue(date::sys_days(date::year(2006) / date::month(6) / date::day(1)) + chrono::hours(11) + chrono::minutes(0) + chrono::seconds(0) + chrono::microseconds(0)),}) },{ "floats", new ArrayValue({new FloatValue(1.1),new FloatValue(2.1),new FloatValue(3.1),}) },{ "ints", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new IntegerValue(3ULL),}) },{ "strings", new ArrayValue({new StringValue("a"),new StringValue("b"),new StringValue("c"),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_string_with_comma)
{
    const string toml{"title = [\n\"Client: XXXX, Job: XXXX\",\n\"Code: XXXX\"\n]\n", 52};

    const Table expected{{ "title", new ArrayValue({new StringValue("Client: XXXX, Job: XXXX"),new StringValue("Code: XXXX"),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_mixed_int_string)
{
    const string toml{"strings-and-ints = [\"hi\", 42]\n", 30};

    const Table expected{{ "strings-and-ints", new ArrayValue({new StringValue("hi"),new IntegerValue(42ULL),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_nested_double)
{
    const string toml{"nest = [\n\t[\n\t\t[\"a\"],\n\t\t[1, 2, [3]]\n\t]\n]\n", 40};

    const Table expected{{ "nest", new ArrayValue({new ArrayValue({new ArrayValue({new StringValue("a"),}),new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new ArrayValue({new IntegerValue(3ULL),}),}),}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_mixed_int_float)
{
    const string toml{"ints-and-floats = [1, 1.1]\n", 27};

    const Table expected{{ "ints-and-floats", new ArrayValue({new IntegerValue(1ULL),new FloatValue(1.1),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_string_quote_comma_2)
{
    const string toml{"title = [ \" \\\", \",]\n", 20};

    const Table expected{{ "title", new ArrayValue({new StringValue(" \", "),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_nested)
{
    const string toml{"nest = [[\"a\"], [\"b\"]]\n", 22};

    const Table expected{{ "nest", new ArrayValue({new ArrayValue({new StringValue("a"),}),new ArrayValue({new StringValue("b"),}),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_bool)
{
    const string toml{"a = [true, false]\n", 18};

    const Table expected{{ "a", new ArrayValue({new BooleanValue(true),new BooleanValue(false),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_strings)
{
    const string toml{"string_array = [ \"all\", 'strings', \"\"\"are the same\"\"\", '''type''']\n", 67};

    const Table expected{{ "string_array", new ArrayValue({new StringValue("all"),new StringValue("strings"),new StringValue("are the same"),new StringValue("type"),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_string_quote_comma)
{
    const string toml{"title = [\n\"Client: \\\"XXXX\\\", Job: XXXX\",\n\"Code: XXXX\"\n]\n", 56};

    const Table expected{{ "title", new ArrayValue({new StringValue("Client: \"XXXX\", Job: XXXX"),new StringValue("Code: XXXX"),}) },};

    assert_parsed(toml, expected);
}


TEST(valid_array_tests, test_hetergeneous)
{
    const string toml{"mixed = [[1, 2], [\"a\", \"b\"], [1.1, 2.1]]\n", 41};

    const Table expected{{ "mixed", new ArrayValue({new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),}),new ArrayValue({new StringValue("a"),new StringValue("b"),}),new ArrayValue({new FloatValue(1.1),new FloatValue(2.1),}),}) },};

    assert_parsed(toml, expected);
}
