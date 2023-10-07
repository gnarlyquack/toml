TEST(valid_inline_table_tests, test_end_in_bool)
{
    const string toml("black = { python=\">3.6\", version=\">=18.9b0\", allow_prereleases=true }\n", 70);

    const Table expected = {{ std::string("black", 5), Value(Table({{ std::string("allow_prereleases", 17), Value(true) },{ std::string("python", 6), Value(std::string(">3.6", 4)) },{ std::string("version", 7), Value(std::string(">=18.9b0", 8)) }})) }};

    assert_parsed(toml, expected);
}
