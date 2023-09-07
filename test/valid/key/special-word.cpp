TEST(valid_key_tests, test_special_word)
{
    const string toml{"false = false\ntrue = 1\ninf = 100000000\nnan = \"ceci n'est pas un nombre\"\n\n", 73};

    const Table expected{{ "false", Value::of_boolean(false) },{ "inf", Value::of_integer(100000000ULL) },{ "nan", Value::of_string("ceci n'est pas un nombre") },{ "true", Value::of_integer(1ULL) }};

    assert_parsed(toml, expected);
}
