TEST(valid_key_tests, test_space)
{
    const string toml("# Keep whitespace inside quotes keys at all positions.\n\"a b\"   = 1\n\" c d \" = 2\n\n[ \" tbl \" ]\n\"\\ttab\\ttab\\t\" = \"tab\"\n", 115);

    const Table expected = {{ std::string(" c d ", 5), Value(INT64_C(2)) },{ std::string(" tbl ", 5), Value(Table({{ std::string("\ttab\ttab\t", 9), Value(std::string("tab", 3)) }})) },{ std::string("a b", 3), Value(INT64_C(1)) }};

    assert_parsed(toml, expected);
}
