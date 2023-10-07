TEST(valid_string_tests, test_simple)
{
    const string toml("answer = \"You are not drinking enough whisky.\"\n", 47);

    const Table expected = {{ std::string("answer", 6), Value(std::string("You are not drinking enough whisky.", 35)) }};

    assert_parsed(toml, expected);
}
