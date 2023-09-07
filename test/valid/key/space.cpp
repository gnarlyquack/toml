TEST(valid_key_tests, test_space)
{
    const string toml{"# Keep whitespace inside quotes keys at all positions.\n\"a b\"   = 1\n\" c d \" = 2\n\n[ \" tbl \" ]\n\"\\ttab\\ttab\\t\" = \"tab\"\n", 115};

    const Table expected{{ " c d ", Value::of_integer(2ULL) },{ " tbl ", Value::of_table({{ "	tab	tab	", Value::of_string("tab") }}) },{ "a b", Value::of_integer(1ULL) }};

    assert_parsed(toml, expected);
}
