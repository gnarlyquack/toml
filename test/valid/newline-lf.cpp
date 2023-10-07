TEST(valid_tests, test_newline_lf)
{
    const string toml("os = \"unix\"\nnewline = \"lf\"\n", 27);

    const Table expected = {{ std::string("newline", 7), Value(std::string("lf", 2)) },{ std::string("os", 2), Value(std::string("unix", 4)) }};

    assert_parsed(toml, expected);
}
