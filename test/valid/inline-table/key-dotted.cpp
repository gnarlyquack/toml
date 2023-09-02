#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_inline_table_tests, test_key_dotted)
{
    const string toml{"inline = {a.b = 42}\n\nmany.dots.here.dot.dot.dot = {a.b.c = 1, a.b.d = 2}\n\na = {   a.b  =  1   }\nb = {   \"a\".\"b\"  =  1   }\nc = {   a   .   b  =  1   }\nd = {   'a'   .   \"b\"  =  1   }\ne = {a.b=1}\n\n[tbl]\na.b.c = {d.e=1}\n\n[tbl.x]\na.b.c = {d.e=1}\n\n[[arr]]\nt = {a.b=1}\nT = {a.b=1}\n\n[[arr]]\nt = {a.b=2}\nT = {a.b=2}\n", 308};

    const Table expected{{ "a", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "arr", new ArrayValue({new TableValue({{ "T", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "t", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },}),new TableValue({{ "T", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(2ULL) },}) },}) },{ "t", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(2ULL) },}) },}) },}),}) },{ "b", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "c", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "d", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "e", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(1ULL) },}) },}) },{ "inline", new TableValue({{ "a", new TableValue({{ "b", new IntegerValue(42ULL) },}) },}) },{ "many", new TableValue({{ "dots", new TableValue({{ "here", new TableValue({{ "dot", new TableValue({{ "dot", new TableValue({{ "dot", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new IntegerValue(1ULL) },{ "d", new IntegerValue(2ULL) },}) },}) },}) },}) },}) },}) },}) },}) },{ "tbl", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "d", new TableValue({{ "e", new IntegerValue(1ULL) },}) },}) },}) },}) },{ "x", new TableValue({{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "d", new TableValue({{ "e", new IntegerValue(1ULL) },}) },}) },}) },}) },}) },}) },};

    assert_parsed(toml, expected);
}
