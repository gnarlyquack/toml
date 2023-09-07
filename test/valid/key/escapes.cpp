TEST(valid_key_tests, test_escapes)
{
    const string toml{"\"\\n\" = \"newline\"\n\"\\u00c0\" = \"latin capital letter A with grave\"\n\"\\\"\" = \"just a quote\"\n\n[\"backsp\\b\\b\"]\n\n[\"\\\"quoted\\\"\"]\nquote = true\n\n[\"a.b\".\"\\u00c0\"]\n", 149};

    const Table expected{{ "\n", Value::of_string("newline") },{ "\"", Value::of_string("just a quote") },{ "\"quoted\"", Value::of_table({{ "quote", Value::of_boolean(true) }}) },{ "a.b", Value::of_table({{ "À", Value::of_table({}) }}) },{ "backsp", Value::of_table({}) },{ "À", Value::of_string("latin capital letter A with grave") }};

    assert_parsed(toml, expected);
}
