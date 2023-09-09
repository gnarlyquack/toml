TEST(valid_tests, test_newline_lf)
{
    const string toml{"os = \"unix\"\nnewline = \"lf\"\n", 27};

    const Table expected{{ "newline", Value(std::string("lf")) },{ "os", Value(std::string("unix")) }};

    assert_parsed(toml, expected);
}
