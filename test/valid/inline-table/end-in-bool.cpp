TEST(valid_inline_table_tests, test_end_in_bool)
{
    const string toml{"black = { python=\">3.6\", version=\">=18.9b0\", allow_prereleases=true }\n", 70};

    const Table expected{{ "black", new TableValue({{ "allow_prereleases", new BooleanValue(true) },{ "python", new StringValue(">3.6") },{ "version", new StringValue(">=18.9b0") },}) },};

    assert_parsed(toml, expected);
}
