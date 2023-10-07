TEST(valid_spec_tests, test_keys_7)
{
    const string toml("3.14159 = \"pi\"\n", 15);

    const Table expected = {{ std::string("3", 1), Value(Table({{ std::string("14159", 5), Value(std::string("pi", 2)) }})) }};

    assert_parsed(toml, expected);
}
