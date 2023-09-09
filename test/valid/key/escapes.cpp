TEST(valid_key_tests, test_escapes)
{
    const string toml{"\"\\n\" = \"newline\"\n\"\\u00c0\" = \"latin capital letter A with grave\"\n\"\\\"\" = \"just a quote\"\n\n[\"backsp\\b\\b\"]\n\n[\"\\\"quoted\\\"\"]\nquote = true\n\n[\"a.b\".\"\\u00c0\"]\n", 149};

    const Table expected{{ "\n", Value(std::string("newline")) },{ "\"", Value(std::string("just a quote")) },{ "\"quoted\"", Value(Table({{ "quote", Value(true) }})) },{ "a.b", Value(Table({{ "À", Value(Table({})) }})) },{ "backsp", Value(Table({})) },{ "À", Value(std::string("latin capital letter A with grave")) }};

    assert_parsed(toml, expected);
}
