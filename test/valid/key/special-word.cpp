TEST(valid_key_tests, test_special_word)
{
    const string toml{"false = false\ntrue = 1\ninf = 100000000\nnan = \"ceci n'est pas un nombre\"\n\n", 73};

    const Table expected{{ "false", new BooleanValue(false) },{ "inf", new IntegerValue(100000000ULL) },{ "nan", new StringValue("ceci n'est pas un nombre") },{ "true", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}
