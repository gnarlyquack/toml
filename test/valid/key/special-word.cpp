TEST(valid_key_tests, test_special_word)
{
    const string toml("false = false\ntrue = 1\ninf = 100000000\nnan = \"ceci n'est pas un nombre\"\n\n", 73);

    const Table expected = {{ std::string("false", 5), Value(false) },{ std::string("inf", 3), Value(INT64_C(100000000)) },{ std::string("nan", 3), Value(std::string("ceci n'est pas un nombre", 24)) },{ std::string("true", 4), Value(INT64_C(1)) }};

    assert_parsed(toml, expected);
}
