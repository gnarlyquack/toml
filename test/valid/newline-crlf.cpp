TEST(valid_tests, test_newline_crlf)
{
    const string toml{"os = \"DOS\"\r\nnewline = \"crlf\"\r\n", 30};

    const Table expected{{ "newline", Value(std::string("crlf")) },{ "os", Value(std::string("DOS")) }};

    assert_parsed(toml, expected);
}
