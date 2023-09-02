#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_key_tests, test_dotted)
{
    const string toml{"# Note: this file contains literal tab characters.\n\nname.first = \"Arthur\"\n\"name\".'last' = \"Dent\"\n\nmany.dots.here.dot.dot.dot = 42\n\n# Space are ignored, and key parts can be quoted.\ncount.a       = 1\ncount . b     = 2\n\"count\".\"c\"   = 3\n\"count\" . \"d\" = 4\n'count'.'e'   = 5\n'count' . 'f' = 6\n\"count\".'g'   = 7\n\"count\" . 'h' = 8\ncount.'i'     = 9\ncount \t.\t 'j'\t   = 10\n\"count\".k     = 11\n\"count\" . l   = 12\n\n[tbl]\na.b.c = 42.666\n\n[a.few.dots]\npolka.dot = \"again?\"\npolka.dance-with = \"Dot\"\n\n[[arr]]\na.b.c=1\na.b.d=2\n\n[[arr]]\na.b.c=3\na.b.d=4\n", 535};

    const Table expected{{ "a", new TableValue({{ "few", new TableValue({{ "dots", new TableValue({{ "polka", new TableValue({{ "dance-with", new StringValue("Dot") },{ "dot", new StringValue("again?") },}) },}) },}) },}) },{ "arr", new ArrayValue({new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new IntegerValue(1ULL) },{ "d", new IntegerValue(2ULL) },}) },}) },}),new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new IntegerValue(3ULL) },{ "d", new IntegerValue(4ULL) },}) },}) },}),}) },{ "count", new TableValue({{ "a", new IntegerValue(1ULL) },{ "b", new IntegerValue(2ULL) },{ "c", new IntegerValue(3ULL) },{ "d", new IntegerValue(4ULL) },{ "e", new IntegerValue(5ULL) },{ "f", new IntegerValue(6ULL) },{ "g", new IntegerValue(7ULL) },{ "h", new IntegerValue(8ULL) },{ "i", new IntegerValue(9ULL) },{ "j", new IntegerValue(10ULL) },{ "k", new IntegerValue(11ULL) },{ "l", new IntegerValue(12ULL) },}) },{ "many", new TableValue({{ "dots", new TableValue({{ "here", new TableValue({{ "dot", new TableValue({{ "dot", new TableValue({{ "dot", new IntegerValue(42ULL) },}) },}) },}) },}) },}) },{ "name", new TableValue({{ "first", new StringValue("Arthur") },{ "last", new StringValue("Dent") },}) },{ "tbl", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new FloatValue(42.666) },}) },}) },}) },};

    assert_parsed(toml, expected);
}
