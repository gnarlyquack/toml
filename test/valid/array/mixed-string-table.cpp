#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_mixed_string_table)
{
    const string toml{"contributors = [\n  \"Foo Bar <foo@example.com>\",\n  { name = \"Baz Qux\", email = \"bazqux@example.com\", url = \"https://example.com/bazqux\" }\n]\n\n# Start with a table as the first element. This tests a case that some libraries\n# might have where they will check if the first entry is a table/map/hash/assoc\n# array and then encode it as a table array. This was a reasonable thing to do\n# before TOML 1.0 since arrays could only contain one type, but now it's no\n# longer.\nmixed = [{k=\"a\"}, \"b\", 1]\n", 492};

    const Table expected{{ "contributors", new ArrayValue({new StringValue("Foo Bar <foo@example.com>"),new TableValue({{ "email", new StringValue("bazqux@example.com") },{ "name", new StringValue("Baz Qux") },{ "url", new StringValue("https://example.com/bazqux") },}),}) },{ "mixed", new ArrayValue({new TableValue({{ "k", new StringValue("a") },}),new StringValue("b"),new IntegerValue(1ULL),}) },};

    assert_parsed(toml, expected);
}
