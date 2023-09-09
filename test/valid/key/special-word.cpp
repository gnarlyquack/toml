TEST(valid_key_tests, test_special_word)
{
    const string toml{"false = false\ntrue = 1\ninf = 100000000\nnan = \"ceci n'est pas un nombre\"\n\n", 73};

    const Table expected{{ "false", Value(false) },{ "inf", Value(INT64_C(100000000)) },{ "nan", Value(std::string("ceci n'est pas un nombre")) },{ "true", Value(INT64_C(1)) }};

    assert_parsed(toml, expected);
}
