TEST(valid_key_tests, test_empty)
{
    const string toml{"\"\" = \"blank\"\n", 13};

    const Table expected{{ "", new StringValue("blank") },};

    assert_parsed(toml, expected);
}
