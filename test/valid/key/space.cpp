TEST(valid_key_tests, test_space)
{
    const string toml("# Keep whitespace inside quotes keys at all positions.\n\"a b\"   = 1\n\" c d \" = 2\n\n[ \" tbl \" ]\n\"\\ttab\\ttab\\t\" = \"tab\"\n", 115);

    const Table expected = {{ " c d ", Value(INT64_C(2)) },{ " tbl ", Value(Table({{ "	tab	tab	", Value(std::string("tab")) }})) },{ "a b", Value(INT64_C(1)) }};

    assert_parsed(toml, expected);
}
