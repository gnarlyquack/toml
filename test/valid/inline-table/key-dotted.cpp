TEST(valid_inline_table_tests, test_key_dotted)
{
    const string toml("inline = {a.b = 42}\n\nmany.dots.here.dot.dot.dot = {a.b.c = 1, a.b.d = 2}\n\na = {   a.b  =  1   }\nb = {   \"a\".\"b\"  =  1   }\nc = {   a   .   b  =  1   }\nd = {   'a'   .   \"b\"  =  1   }\ne = {a.b=1}\n\n[tbl]\na.b.c = {d.e=1}\n\n[tbl.x]\na.b.c = {d.e=1}\n\n[[arr]]\nt = {a.b=1}\nT = {a.b=1}\n\n[[arr]]\nt = {a.b=2}\nT = {a.b=2}\n", 308);

    const Table expected = {{ "a", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(1)) }})) }})) },{ "arr", Value(Array({Value(Table({{ "T", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(1)) }})) }})) },{ "t", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(1)) }})) }})) }})),Value(Table({{ "T", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(2)) }})) }})) },{ "t", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(2)) }})) }})) }}))})) },{ "b", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(1)) }})) }})) },{ "c", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(1)) }})) }})) },{ "d", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(1)) }})) }})) },{ "e", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(1)) }})) }})) },{ "inline", Value(Table({{ "a", Value(Table({{ "b", Value(INT64_C(42)) }})) }})) },{ "many", Value(Table({{ "dots", Value(Table({{ "here", Value(Table({{ "dot", Value(Table({{ "dot", Value(Table({{ "dot", Value(Table({{ "a", Value(Table({{ "b", Value(Table({{ "c", Value(INT64_C(1)) },{ "d", Value(INT64_C(2)) }})) }})) }})) }})) }})) }})) }})) }})) },{ "tbl", Value(Table({{ "a", Value(Table({{ "b", Value(Table({{ "c", Value(Table({{ "d", Value(Table({{ "e", Value(INT64_C(1)) }})) }})) }})) }})) },{ "x", Value(Table({{ "a", Value(Table({{ "b", Value(Table({{ "c", Value(Table({{ "d", Value(Table({{ "e", Value(INT64_C(1)) }})) }})) }})) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
