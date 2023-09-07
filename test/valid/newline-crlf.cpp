TEST(valid_tests, test_newline_crlf)
{
    const string toml{"os = \"DOS\"\r\nnewline = \"crlf\"\r\n", 30};

    const Table expected{{ "newline", Value::of_string("crlf") },{ "os", Value::of_string("DOS") }};

    assert_parsed(toml, expected);
}
