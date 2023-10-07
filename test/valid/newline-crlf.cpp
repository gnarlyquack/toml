TEST(valid_tests, test_newline_crlf)
{
    const string toml("os = \"DOS\"\r\nnewline = \"crlf\"\r\n", 30);

    const Table expected = {{ std::string("newline", 7), Value(std::string("crlf", 4)) },{ std::string("os", 2), Value(std::string("DOS", 3)) }};

    assert_parsed(toml, expected);
}
