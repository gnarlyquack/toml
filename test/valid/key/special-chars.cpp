TEST(valid_key_tests, test_special_chars)
{
    const string toml("\"=~!@$^&*()_+-`1234567890[]|/?><.,;:'=\" = 1\n", 44);

    const Table expected = {{ std::string("=~!@$^&*()_+-`1234567890[]|/?><.,;:'=", 37), Value(INT64_C(1)) }};

    assert_parsed(toml, expected);
}
