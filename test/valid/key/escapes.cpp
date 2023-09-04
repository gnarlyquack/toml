TEST(valid_key_tests, test_escapes)
{
    const string toml{"\"\\n\" = \"newline\"\n\"\\u00c0\" = \"latin capital letter A with grave\"\n\"\\\"\" = \"just a quote\"\n\n[\"backsp\\b\\b\"]\n\n[\"\\\"quoted\\\"\"]\nquote = true\n\n[\"a.b\".\"\\u00c0\"]\n", 149};

    const Table expected{{ "\n", new StringValue("newline") },{ "\"", new StringValue("just a quote") },{ "\"quoted\"", new TableValue({{ "quote", new BooleanValue(true) },}) },{ "a.b", new TableValue({{ "À", new TableValue({}) },}) },{ "backsp", new TableValue({}) },{ "À", new StringValue("latin capital letter A with grave") },};

    assert_parsed(toml, expected);
}
