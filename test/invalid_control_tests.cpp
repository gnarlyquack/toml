#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_comment_us)
{
    const string toml{"comment-us = \"ctrl-_\" # \037\n", 26};

    const vector<Error> expected{{ 1, 25, "Unicode character U+001F not allowed in comment." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_multi_lf)
{
    const string toml{"multi-lf = \"\"\"null\020\"\"\"\n", 23};

    const vector<Error> expected{{ 1, 19, "Unicode character U+0010 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawmulti_del)
{
    const string toml{"rawmulti-del = '''null\177'''\n", 27};

    const vector<Error> expected{{ 1, 23, "Unicode character U+007F not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_bare_null)
{
    const string toml{"bare-null = \"some value\" \000\n", 27};

    const vector<Error> expected{{ 1, 26, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawmulti_us)
{
    const string toml{"rawmulti-us = '''null\037'''\n", 26};

    const vector<Error> expected{{ 1, 22, "Unicode character U+001F not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawstring_del)
{
    const string toml{"rawstring-del = 'null\177'\n", 24};

    const vector<Error> expected{{ 1, 22, "Unicode character U+007F not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_bare_cr)
{
    const string toml{"# The following line contains a single carriage return control character\r\n\r", 75};

    const vector<Error> expected{{ 2, 1, "Invalid key: \r." },{ 2, 2, "Missing '=' between key and value." },{ 2, 2, "Missing value." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_multi_us)
{
    const string toml{"multi-us = \"\"\"null\037\"\"\"\n", 23};

    const vector<Error> expected{{ 1, 19, "Unicode character U+001F not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawstring_lf)
{
    const string toml{"rawstring-lf = 'null\020'\n", 23};

    const vector<Error> expected{{ 1, 21, "Unicode character U+0010 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_string_del)
{
    const string toml{"string-del = \"null\177\"\n", 21};

    const vector<Error> expected{{ 1, 19, "Unicode character U+007F not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_bare_formfeed)
{
    const string toml{"bare-formfeed = \f\n", 18};

    const vector<Error> expected{{ 1, 17, "Invalid value: \f." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_comment_cr)
{
    const string toml{"comment-cr = \"Carriage return in comment\" # \ra=1\n", 49};

    const vector<Error> expected{{ 1, 45, "Unicode character U+000D not allowed in comment." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawmulti_lf)
{
    const string toml{"rawmulti-lf = '''null\020'''\n", 26};

    const vector<Error> expected{{ 1, 22, "Unicode character U+0010 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_multi_del)
{
    const string toml{"multi-del = \"\"\"null\177\"\"\"\n", 24};

    const vector<Error> expected{{ 1, 20, "Unicode character U+007F not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_string_bs)
{
    const string toml{"string-bs = \"backspace\b\"\n", 25};

    const vector<Error> expected{{ 1, 23, "Unicode character U+0008 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_bare_vertical_tab)
{
    const string toml{"bare-vertical-tab = \v\n", 22};

    const vector<Error> expected{{ 1, 21, "Invalid value: \v." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_comment_null)
{
    const string toml{"comment-null = \"null\" # \000\n", 26};

    const vector<Error> expected{{ 1, 25, "Unicode character U+0000 not allowed in comment." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_string_null)
{
    const string toml{"string-null = \"null\000\"\n", 22};

    const vector<Error> expected{{ 1, 20, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawstring_us)
{
    const string toml{"rawstring-us = 'null\037'\n", 23};

    const vector<Error> expected{{ 1, 21, "Unicode character U+001F not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_string_lf)
{
    const string toml{"string-lf = \"null\020\"\n", 20};

    const vector<Error> expected{{ 1, 18, "Unicode character U+0010 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawstring_null)
{
    const string toml{"rawstring-null = 'null\000'\n", 25};

    const vector<Error> expected{{ 1, 23, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_multi_null)
{
    const string toml{"multi-null = \"\"\"null\000\"\"\"\n", 25};

    const vector<Error> expected{{ 1, 21, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_rawmulti_null)
{
    const string toml{"rawmulti-null = '''null\000'''\n", 28};

    const vector<Error> expected{{ 1, 24, "Unicode character U+0000 not allowed in string." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_comment_lf)
{
    const string toml{"comment-lf = \"ctrl-P\" # \020\n", 26};

    const vector<Error> expected{{ 1, 25, "Unicode character U+0010 not allowed in comment." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_comment_del)
{
    const string toml{"comment-del = \"0x7f\" # \177\n", 25};

    const vector<Error> expected{{ 1, 24, "Unicode character U+007F not allowed in comment." },};

    assert_errors(toml, expected);
}


TEST(invalid_control_tests, test_string_us)
{
    const string toml{"string-us = \"null\037\"\n", 20};

    const vector<Error> expected{{ 1, 18, "Unicode character U+001F not allowed in string." },};

    assert_errors(toml, expected);
}
