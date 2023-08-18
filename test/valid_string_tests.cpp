#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_empty)
{
    const string toml = "answer = \"\"\n";

    const Table expected{{ "answer", new StringValue("") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_multiline_quotes)
{
    const string toml = "# Make sure that quotes inside multiline strings are allowed, including right\n# after the opening '''/\"\"\" and before the closing '''/\"\"\"\n\nlit_one = ''''one quote''''\nlit_two = '''''two quotes'''''\nlit_one_space = ''' 'one quote' '''\nlit_two_space = ''' ''two quotes'' '''\n\none = \"\"\"\"one quote\"\"\"\"\ntwo = \"\"\"\"\"two quotes\"\"\"\"\"\none_space = \"\"\" \"one quote\" \"\"\"\ntwo_space = \"\"\" \"\"two quotes\"\" \"\"\"\n\nmismatch1 = \"\"\"aaa'''bbb\"\"\"\nmismatch2 = '''aaa\"\"\"bbb'''\n\n# Three opening \"\"\", then one escaped \", then two \"\" (allowed), and then three\n# closing \"\"\"\nescaped = \"\"\"lol\\\"\"\"\"\"\"\n";

    const Table expected{{ "escaped", new StringValue("lol\"\"\"") },{ "lit_one", new StringValue("'one quote'") },{ "lit_one_space", new StringValue(" 'one quote' ") },{ "lit_two", new StringValue("''two quotes''") },{ "lit_two_space", new StringValue(" ''two quotes'' ") },{ "mismatch1", new StringValue("aaa'''bbb") },{ "mismatch2", new StringValue("aaa\"\"\"bbb") },{ "one", new StringValue("\"one quote\"") },{ "one_space", new StringValue(" \"one quote\" ") },{ "two", new StringValue("\"\"two quotes\"\"") },{ "two_space", new StringValue(" \"\"two quotes\"\" ") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_multiline_escaped_crlf)
{
    const string toml = "# The following line should be an unescaped backslash followed by a Windows\r\n# newline sequence (\"\\r\\n\")\r\n0=\"\"\"\\\r\n\"\"\"\r\n";

    const Table expected{{ "0", new StringValue("") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_raw_multiline)
{
    const string toml = "# Single ' should be allowed.\noneline = '''This string has a ' quote character.'''\n\n# A newline immediately following the opening delimiter will be trimmed.\nfirstnl = '''\nThis string has a ' quote character.'''\n\n# All other whitespace and newline characters remain intact.\nmultiline = '''\nThis string\nhas ' a quote character\nand more than\none newline\nin it.'''\n\n# Tab character in literal string does not need to be escaped\nmultiline_with_tab = '''First line\n\t Followed by a tab'''\n";

    const Table expected{{ "firstnl", new StringValue("This string has a ' quote character.") },{ "multiline", new StringValue("This string\nhas ' a quote character\nand more than\none newline\nin it.") },{ "oneline", new StringValue("This string has a ' quote character.") },{ "multiline_with_tab", new StringValue("First line\n\t Followed by a tab") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_double_quote_escape)
{
    const string toml = "test = \"\\\"one\\\"\"\n";

    const Table expected{{ "test", new StringValue("\"one\"") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_nl)
{
    const string toml = "nl_mid = \"val\\nue\"\nnl_end = \"\"\"value\\n\"\"\"\n\nlit_nl_end = '''value\\n'''\nlit_nl_mid = 'val\\nue'\nlit_nl_uni = 'val\\ue'\n";

    const Table expected{{ "lit_nl_end", new StringValue("value\\n") },{ "lit_nl_mid", new StringValue("val\\nue") },{ "lit_nl_uni", new StringValue("val\\ue") },{ "nl_end", new StringValue("value\n") },{ "nl_mid", new StringValue("val\nue") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_multiline)
{
    const string toml = "# NOTE: this file includes some literal tab characters.\n\nmultiline_empty_one = \"\"\"\"\"\"\n\n# A newline immediately following the opening delimiter will be trimmed.\nmultiline_empty_two = \"\"\"\n\"\"\"\n\n# \\ at the end of line trims newlines as well; note that last \\ is followed by\n# two spaces, which are ignored.\nmultiline_empty_three = \"\"\"\\\n    \"\"\"\nmultiline_empty_four = \"\"\"\\\n   \\\n   \\  \n   \"\"\"\n\nequivalent_one = \"The quick brown fox jumps over the lazy dog.\"\nequivalent_two = \"\"\"\nThe quick brown \\\n\n\n  fox jumps over \\\n    the lazy dog.\"\"\"\n\nequivalent_three = \"\"\"\\\n       The quick brown \\\n       fox jumps over \\\n       the lazy dog.\\\n       \"\"\"\n\nwhitespace-after-bs = \"\"\"\\\n       The quick brown \\\n       fox jumps over \\   \n       the lazy dog.\\\t\n       \"\"\"\n\nno-space = \"\"\"a\\\n    b\"\"\"\n\n# Has tab character.\nkeep-ws-before = \"\"\"a   \t\\\n   b\"\"\"\n\nescape-bs-1 = \"\"\"a \\\\\nb\"\"\"\n\nescape-bs-2 = \"\"\"a \\\\\\\nb\"\"\"\n\nescape-bs-3 = \"\"\"a \\\\\\\\\n  b\"\"\"\n";

    const Table expected{{ "equivalent_one", new StringValue("The quick brown fox jumps over the lazy dog.") },{ "equivalent_three", new StringValue("The quick brown fox jumps over the lazy dog.") },{ "equivalent_two", new StringValue("The quick brown fox jumps over the lazy dog.") },{ "escape-bs-1", new StringValue("a \\\nb") },{ "escape-bs-2", new StringValue("a \\b") },{ "escape-bs-3", new StringValue("a \\\\\n  b") },{ "keep-ws-before", new StringValue("a   \tb") },{ "multiline_empty_four", new StringValue("") },{ "multiline_empty_one", new StringValue("") },{ "multiline_empty_three", new StringValue("") },{ "multiline_empty_two", new StringValue("") },{ "no-space", new StringValue("ab") },{ "whitespace-after-bs", new StringValue("The quick brown fox jumps over the lazy dog.") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_escape_tricky)
{
    const string toml = "end_esc = \"String does not end here\\\" but ends here\\\\\"\nlit_end_esc = 'String ends here\\'\n\nmultiline_unicode = \"\"\"\n\\u00a0\"\"\"\n\nmultiline_not_unicode = \"\"\"\n\\\\u0041\"\"\"\n\nmultiline_end_esc = \"\"\"When will it end? \\\"\"\"...\"\"\\\" should be here\\\"\"\"\"\n\nlit_multiline_not_unicode = '''\n\\u007f'''\n\nlit_multiline_end = '''There is no escape\\'''\n";

    const Table expected{{ "end_esc", new StringValue("String does not end here\" but ends here\\") },{ "lit_end_esc", new StringValue("String ends here\\") },{ "lit_multiline_end", new StringValue("There is no escape\\") },{ "lit_multiline_not_unicode", new StringValue("\\u007f") },{ "multiline_end_esc", new StringValue("When will it end? \"\"\"...\"\"\" should be here\"") },{ "multiline_not_unicode", new StringValue("\\u0041") },{ "multiline_unicode", new StringValue("\u00A0") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_unicode_literal)
{
    const string toml = "answer = \"\316\264\"\n";

    const Table expected{{ "answer", new StringValue("δ") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_escapes)
{
    const string toml = "backspace = \"This string has a \\b backspace character.\"\ntab = \"This string has a \\t tab character.\"\nnewline = \"This string has a \\n new line character.\"\nformfeed = \"This string has a \\f form feed character.\"\ncarriage = \"This string has a \\r carriage return character.\"\nquote = \"This string has a \\\" quote character.\"\nbackslash = \"This string has a \\\\ backslash character.\"\nnotunicode1 = \"This string does not have a unicode \\\\u escape.\"\nnotunicode2 = \"This string does not have a unicode \\u005Cu escape.\"\nnotunicode3 = \"This string does not have a unicode \\\\u0075 escape.\"\nnotunicode4 = \"This string does not have a unicode \\\\\\u0075 escape.\"\ndelete = \"This string has a \\u007F delete control code.\"\nunitseparator = \"This string has a \\u001F unit separator control code.\"\n";

    const Table expected{{ "backslash", new StringValue("This string has a \\ backslash character.") },{ "backspace", new StringValue("This string has a \b backspace character.") },{ "carriage", new StringValue("This string has a \r carriage return character.") },{ "delete", new StringValue("This string has a \u007F delete control code.") },{ "formfeed", new StringValue("This string has a \f form feed character.") },{ "newline", new StringValue("This string has a \n new line character.") },{ "notunicode1", new StringValue("This string does not have a unicode \\u escape.") },{ "notunicode2", new StringValue("This string does not have a unicode \\u escape.") },{ "notunicode3", new StringValue("This string does not have a unicode \\u0075 escape.") },{ "notunicode4", new StringValue("This string does not have a unicode \\u escape.") },{ "quote", new StringValue("This string has a \" quote character.") },{ "tab", new StringValue("This string has a \t tab character.") },{ "unitseparator", new StringValue("This string has a \u001F unit separator control code.") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_escaped_escape)
{
    const string toml = "answer = \"\\\\x64\"\n";

    const Table expected{{ "answer", new StringValue("\\x64") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_unicode_escape)
{
    const string toml = "answer4 = \"\\u03B4\"\nanswer8 = \"\\U000003B4\"\n";

    const Table expected{{ "answer4", new StringValue("δ") },{ "answer8", new StringValue("δ") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_raw)
{
    const string toml = "backspace = 'This string has a \\b backspace character.'\ntab = 'This string has a \\t tab character.'\nunescaped_tab = 'This string has an \t unescaped tab character.'\nnewline = 'This string has a \\n new line character.'\nformfeed = 'This string has a \\f form feed character.'\ncarriage = 'This string has a \\r carriage return character.'\nslash = 'This string has a \\/ slash character.'\nbackslash = 'This string has a \\\\ backslash character.'\n";

    const Table expected{{ "backslash", new StringValue("This string has a \\\\ backslash character.") },{ "backspace", new StringValue("This string has a \\b backspace character.") },{ "carriage", new StringValue("This string has a \\r carriage return character.") },{ "formfeed", new StringValue("This string has a \\f form feed character.") },{ "newline", new StringValue("This string has a \\n new line character.") },{ "slash", new StringValue("This string has a \\/ slash character.") },{ "tab", new StringValue("This string has a \\t tab character.") },{ "unescaped_tab", new StringValue("This string has an \t unescaped tab character.") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_with_pound)
{
    const string toml = "pound = \"We see no # comments here.\"\npoundcomment = \"But there are # some comments here.\" # Did I # mess you up?\n";

    const Table expected{{ "pound", new StringValue("We see no # comments here.") },{ "poundcomment", new StringValue("But there are # some comments here.") },};

    assert_parsed(toml, expected);
}


TEST(valid_string_tests, test_simple)
{
    const string toml = "answer = \"You are not drinking enough whisky.\"\n";

    const Table expected{{ "answer", new StringValue("You are not drinking enough whisky.") },};

    assert_parsed(toml, expected);
}
