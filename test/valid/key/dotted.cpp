TEST(valid_key_tests, test_dotted)
{
    const string toml{"# Note: this file contains literal tab characters.\n\nname.first = \"Arthur\"\n\"name\".'last' = \"Dent\"\n\nmany.dots.here.dot.dot.dot = 42\n\n# Space are ignored, and key parts can be quoted.\ncount.a       = 1\ncount . b     = 2\n\"count\".\"c\"   = 3\n\"count\" . \"d\" = 4\n'count'.'e'   = 5\n'count' . 'f' = 6\n\"count\".'g'   = 7\n\"count\" . 'h' = 8\ncount.'i'     = 9\ncount \t.\t 'j'\t   = 10\n\"count\".k     = 11\n\"count\" . l   = 12\n\n[tbl]\na.b.c = 42.666\n\n[a.few.dots]\npolka.dot = \"again?\"\npolka.dance-with = \"Dot\"\n\n[[arr]]\na.b.c=1\na.b.d=2\n\n[[arr]]\na.b.c=3\na.b.d=4\n", 535};

    const Table expected{{ "a", Value::of_table({{ "few", Value::of_table({{ "dots", Value::of_table({{ "polka", Value::of_table({{ "dance-with", Value::of_string("Dot") },{ "dot", Value::of_string("again?") }}) }}) }}) }}) },{ "arr", Value::of_array({Value::of_table({{ "a", Value::of_table({{ "b", Value::of_table({{ "c", Value::of_integer(1ULL) },{ "d", Value::of_integer(2ULL) }}) }}) }}),Value::of_table({{ "a", Value::of_table({{ "b", Value::of_table({{ "c", Value::of_integer(3ULL) },{ "d", Value::of_integer(4ULL) }}) }}) }})}) },{ "count", Value::of_table({{ "a", Value::of_integer(1ULL) },{ "b", Value::of_integer(2ULL) },{ "c", Value::of_integer(3ULL) },{ "d", Value::of_integer(4ULL) },{ "e", Value::of_integer(5ULL) },{ "f", Value::of_integer(6ULL) },{ "g", Value::of_integer(7ULL) },{ "h", Value::of_integer(8ULL) },{ "i", Value::of_integer(9ULL) },{ "j", Value::of_integer(10ULL) },{ "k", Value::of_integer(11ULL) },{ "l", Value::of_integer(12ULL) }}) },{ "many", Value::of_table({{ "dots", Value::of_table({{ "here", Value::of_table({{ "dot", Value::of_table({{ "dot", Value::of_table({{ "dot", Value::of_integer(42ULL) }}) }}) }}) }}) }}) },{ "name", Value::of_table({{ "first", Value::of_string("Arthur") },{ "last", Value::of_string("Dent") }}) },{ "tbl", Value::of_table({{ "a", Value::of_table({{ "b", Value::of_table({{ "c", Value::of_float(42.666) }}) }}) }}) }};

    assert_parsed(toml, expected);
}
