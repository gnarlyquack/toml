TEST(valid_tests, test_newline_lf)
{
    const string toml{"os = \"unix\"\nnewline = \"lf\"\n", 27};

    const Table expected{{ "newline", new StringValue("lf") },{ "os", new StringValue("unix") },};

    assert_parsed(toml, expected);
}
