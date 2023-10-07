TEST(valid_key_tests, test_escapes)
{
    const string toml("\"\\n\" = \"newline\"\n\"\\u00c0\" = \"latin capital letter A with grave\"\n\"\\\"\" = \"just a quote\"\n\n[\"backsp\\b\\b\"]\n\n[\"\\\"quoted\\\"\"]\nquote = true\n\n[\"a.b\".\"\\u00c0\"]\n", 149);

    const Table expected = {{ std::string("\n", 1), Value(std::string("newline", 7)) },{ std::string("\"", 1), Value(std::string("just a quote", 12)) },{ std::string("\"quoted\"", 8), Value(Table({{ std::string("quote", 5), Value(true) }})) },{ std::string("a.b", 3), Value(Table({{ std::string("À", 2), Value(Table({})) }})) },{ std::string("backsp\b\b", 8), Value(Table({})) },{ std::string("À", 2), Value(std::string("latin capital letter A with grave", 33)) }};

    assert_parsed(toml, expected);
}
