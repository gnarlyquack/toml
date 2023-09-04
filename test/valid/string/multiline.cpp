#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_multiline)
{
    const string toml{"# NOTE: this file includes some literal tab characters.\n\nmultiline_empty_one = \"\"\"\"\"\"\n\n# A newline immediately following the opening delimiter will be trimmed.\nmultiline_empty_two = \"\"\"\n\"\"\"\n\n# \\ at the end of line trims newlines as well; note that last \\ is followed by\n# two spaces, which are ignored.\nmultiline_empty_three = \"\"\"\\\n    \"\"\"\nmultiline_empty_four = \"\"\"\\\n   \\\n   \\  \n   \"\"\"\n\nequivalent_one = \"The quick brown fox jumps over the lazy dog.\"\nequivalent_two = \"\"\"\nThe quick brown \\\n\n\n  fox jumps over \\\n    the lazy dog.\"\"\"\n\nequivalent_three = \"\"\"\\\n       The quick brown \\\n       fox jumps over \\\n       the lazy dog.\\\n       \"\"\"\n\nwhitespace-after-bs = \"\"\"\\\n       The quick brown \\\n       fox jumps over \\   \n       the lazy dog.\\\t\n       \"\"\"\n\nno-space = \"\"\"a\\\n    b\"\"\"\n\n# Has tab character.\nkeep-ws-before = \"\"\"a   \t\\\n   b\"\"\"\n\nescape-bs-1 = \"\"\"a \\\\\nb\"\"\"\n\nescape-bs-2 = \"\"\"a \\\\\\\nb\"\"\"\n\nescape-bs-3 = \"\"\"a \\\\\\\\\n  b\"\"\"\n", 927};

    const Table expected{{ "equivalent_one", new StringValue("The quick brown fox jumps over the lazy dog.") },{ "equivalent_three", new StringValue("The quick brown fox jumps over the lazy dog.") },{ "equivalent_two", new StringValue("The quick brown fox jumps over the lazy dog.") },{ "escape-bs-1", new StringValue("a \\\nb") },{ "escape-bs-2", new StringValue("a \\b") },{ "escape-bs-3", new StringValue("a \\\\\n  b") },{ "keep-ws-before", new StringValue("a   \tb") },{ "multiline_empty_four", new StringValue("") },{ "multiline_empty_one", new StringValue("") },{ "multiline_empty_three", new StringValue("") },{ "multiline_empty_two", new StringValue("") },{ "no-space", new StringValue("ab") },{ "whitespace-after-bs", new StringValue("The quick brown fox jumps over the lazy dog.") },};

    assert_parsed(toml, expected);
}