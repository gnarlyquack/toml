TEST(valid_inline_table_tests, test_end_in_bool)
{
    const string toml("black = { python=\">3.6\", version=\">=18.9b0\", allow_prereleases=true }\n", 70);

    const Table expected = {{ "black", Value(Table({{ "allow_prereleases", Value(true) },{ "python", Value(std::string(">3.6")) },{ "version", Value(std::string(">=18.9b0")) }})) }};

    assert_parsed(toml, expected);
}
