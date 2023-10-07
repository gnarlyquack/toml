TEST(valid_spec_tests, test_table_9)
{
    const string toml("[fruit]\napple.color = \"red\"\napple.taste.sweet = true\n\n# [fruit.apple]  # INVALID\n# [fruit.apple.taste]  # INVALID\n\n[fruit.apple.texture]  # you can add sub-tables\nsmooth = true\n", 177);

    const Table expected = {{ std::string("fruit", 5), Value(Table({{ std::string("apple", 5), Value(Table({{ std::string("color", 5), Value(std::string("red", 3)) },{ std::string("taste", 5), Value(Table({{ std::string("sweet", 5), Value(true) }})) },{ std::string("texture", 7), Value(Table({{ std::string("smooth", 6), Value(true) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
