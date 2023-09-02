#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_escapes)
{
    const string toml{"backspace = \"This string has a \\b backspace character.\"\ntab = \"This string has a \\t tab character.\"\nnewline = \"This string has a \\n new line character.\"\nformfeed = \"This string has a \\f form feed character.\"\ncarriage = \"This string has a \\r carriage return character.\"\nquote = \"This string has a \\\" quote character.\"\nbackslash = \"This string has a \\\\ backslash character.\"\nnotunicode1 = \"This string does not have a unicode \\\\u escape.\"\nnotunicode2 = \"This string does not have a unicode \\u005Cu escape.\"\nnotunicode3 = \"This string does not have a unicode \\\\u0075 escape.\"\nnotunicode4 = \"This string does not have a unicode \\\\\\u0075 escape.\"\ndelete = \"This string has a \\u007F delete control code.\"\nunitseparator = \"This string has a \\u001F unit separator control code.\"\n", 771};

    const Table expected{{ "backslash", new StringValue("This string has a \\ backslash character.") },{ "backspace", new StringValue("This string has a \b backspace character.") },{ "carriage", new StringValue("This string has a \r carriage return character.") },{ "delete", new StringValue("This string has a \u007F delete control code.") },{ "formfeed", new StringValue("This string has a \f form feed character.") },{ "newline", new StringValue("This string has a \n new line character.") },{ "notunicode1", new StringValue("This string does not have a unicode \\u escape.") },{ "notunicode2", new StringValue("This string does not have a unicode \\u escape.") },{ "notunicode3", new StringValue("This string does not have a unicode \\u0075 escape.") },{ "notunicode4", new StringValue("This string does not have a unicode \\u escape.") },{ "quote", new StringValue("This string has a \" quote character.") },{ "tab", new StringValue("This string has a \t tab character.") },{ "unitseparator", new StringValue("This string has a \u001F unit separator control code.") },};

    assert_parsed(toml, expected);
}
