#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_key_tests, test_empty)
{
    const string toml = "\"\" = \"blank\"\n";

    const Table expected{{ "", new StringValue("blank") },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_alphanum)
{
    const string toml = "alpha = \"a\"\n123 = \"num\"\n000111 = \"leading\"\n10e3 = \"false float\"\none1two2 = \"mixed\"\nwith-dash = \"dashed\"\nunder_score = \"___\"\n34-11 = 23\n\n[2018_10]\n001 = 1\n\n[a-a-a]\n_ = false\n";

    const Table expected{{ "000111", new StringValue("leading") },{ "10e3", new StringValue("false float") },{ "123", new StringValue("num") },{ "2018_10", new TableValue({{ "001", new IntegerValue(1ULL) },}) },{ "34-11", new IntegerValue(23ULL) },{ "a-a-a", new TableValue({{ "_", new BooleanValue(false) },}) },{ "alpha", new StringValue("a") },{ "one1two2", new StringValue("mixed") },{ "under_score", new StringValue("___") },{ "with-dash", new StringValue("dashed") },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_numeric_dotted)
{
    const string toml = "1.2 = 3\n";

    const Table expected{{ "1", new TableValue({{ "2", new IntegerValue(3ULL) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_special_chars)
{
    const string toml = "\"=~!@$^&*()_+-`1234567890[]|/?><.,;:'=\" = 1\n";

    const Table expected{{ "=~!@$^&*()_+-`1234567890[]|/?><.,;:'=", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_numeric)
{
    const string toml = "1 = 1\n";

    const Table expected{{ "1", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_equals_nospace)
{
    const string toml = "answer=42\n";

    const Table expected{{ "answer", new IntegerValue(42ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_case_sensitive)
{
    const string toml = "sectioN = \"NN\"\n\n[section]\nname = \"lower\"\nNAME = \"upper\"\nName = \"capitalized\"\n\n[Section]\nname = \"different section!!\"\n\"\316\274\" = \"greek small letter mu\"\n\"\316\234\" = \"greek capital letter MU\"\nM = \"latin letter M\"\n\n";

    const Table expected{{ "Section", new TableValue({{ "M", new StringValue("latin letter M") },{ "name", new StringValue("different section!!") },{ "Μ", new StringValue("greek capital letter MU") },{ "μ", new StringValue("greek small letter mu") },}) },{ "sectioN", new StringValue("NN") },{ "section", new TableValue({{ "NAME", new StringValue("upper") },{ "Name", new StringValue("capitalized") },{ "name", new StringValue("lower") },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_dotted)
{
    const string toml = "# Note: this file contains literal tab characters.\n\nname.first = \"Arthur\"\n\"name\".'last' = \"Dent\"\n\nmany.dots.here.dot.dot.dot = 42\n\n# Space are ignored, and key parts can be quoted.\ncount.a       = 1\ncount . b     = 2\n\"count\".\"c\"   = 3\n\"count\" . \"d\" = 4\n'count'.'e'   = 5\n'count' . 'f' = 6\n\"count\".'g'   = 7\n\"count\" . 'h' = 8\ncount.'i'     = 9\ncount \t.\t 'j'\t   = 10\n\"count\".k     = 11\n\"count\" . l   = 12\n\n[tbl]\na.b.c = 42.666\n\n[a.few.dots]\npolka.dot = \"again?\"\npolka.dance-with = \"Dot\"\n\n[[arr]]\na.b.c=1\na.b.d=2\n\n[[arr]]\na.b.c=3\na.b.d=4\n";

    const Table expected{{ "a", new TableValue({{ "few", new TableValue({{ "dots", new TableValue({{ "polka", new TableValue({{ "dance-with", new StringValue("Dot") },{ "dot", new StringValue("again?") },}) },}) },}) },}) },{ "arr", new ArrayValue({new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new IntegerValue(1ULL) },{ "d", new IntegerValue(2ULL) },}) },}) },}),new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new IntegerValue(3ULL) },{ "d", new IntegerValue(4ULL) },}) },}) },}),}) },{ "count", new TableValue({{ "a", new IntegerValue(1ULL) },{ "b", new IntegerValue(2ULL) },{ "c", new IntegerValue(3ULL) },{ "d", new IntegerValue(4ULL) },{ "e", new IntegerValue(5ULL) },{ "f", new IntegerValue(6ULL) },{ "g", new IntegerValue(7ULL) },{ "h", new IntegerValue(8ULL) },{ "i", new IntegerValue(9ULL) },{ "j", new IntegerValue(10ULL) },{ "k", new IntegerValue(11ULL) },{ "l", new IntegerValue(12ULL) },}) },{ "many", new TableValue({{ "dots", new TableValue({{ "here", new TableValue({{ "dot", new TableValue({{ "dot", new TableValue({{ "dot", new IntegerValue(42ULL) },}) },}) },}) },}) },}) },{ "name", new TableValue({{ "first", new StringValue("Arthur") },{ "last", new StringValue("Dent") },}) },{ "tbl", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new FloatValue(42.666) },}) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_escapes)
{
    const string toml = "\"\\n\" = \"newline\"\n\"\\u00c0\" = \"latin capital letter A with grave\"\n\"\\\"\" = \"just a quote\"\n\n[\"backsp\\b\\b\"]\n\n[\"\\\"quoted\\\"\"]\nquote = true\n\n[\"a.b\".\"\\u00c0\"]\n";

    const Table expected{{ "\n", new StringValue("newline") },{ "\"", new StringValue("just a quote") },{ "\"quoted\"", new TableValue({{ "quote", new BooleanValue(true) },}) },{ "a.b", new TableValue({{ "À", new TableValue({}) },}) },{ "backsp", new TableValue({}) },{ "À", new StringValue("latin capital letter A with grave") },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_space)
{
    const string toml = "# Keep whitespace inside quotes keys at all positions.\n\"a b\"   = 1\n\" c d \" = 2\n\n[ \" tbl \" ]\n\"\\ttab\\ttab\\t\" = \"tab\"\n";

    const Table expected{{ " c d ", new IntegerValue(2ULL) },{ " tbl ", new TableValue({{ "	tab	tab	", new StringValue("tab") },}) },{ "a b", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_quoted_dots)
{
    const string toml = "plain = 1\n\"with.dot\" = 2\n\n[plain_table]\nplain = 3\n\"with.dot\" = 4\n\n[table.withdot]\nplain = 5\n\"key.with.dots\" = 6\n";

    const Table expected{{ "plain", new IntegerValue(1ULL) },{ "plain_table", new TableValue({{ "plain", new IntegerValue(3ULL) },{ "with.dot", new IntegerValue(4ULL) },}) },{ "table", new TableValue({{ "withdot", new TableValue({{ "key.with.dots", new IntegerValue(6ULL) },{ "plain", new IntegerValue(5ULL) },}) },}) },{ "with.dot", new IntegerValue(2ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_key_tests, test_special_word)
{
    const string toml = "false = false\ntrue = 1\ninf = 100000000\nnan = \"ceci n'est pas un nombre\"\n\n";

    const Table expected{{ "false", new BooleanValue(false) },{ "inf", new IntegerValue(100000000ULL) },{ "nan", new StringValue("ceci n'est pas un nombre") },{ "true", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}
