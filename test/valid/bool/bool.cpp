TEST(valid_bool_tests, test_bool)
{
    const string toml{"t = true\nf = false\n", 19};

    const Table expected{{ "f", new BooleanValue(false) },{ "t", new BooleanValue(true) },};

    assert_parsed(toml, expected);
}
