TEST(valid_inline_table_tests, test_end_in_bool)
{
    const string toml{"black = { python=\">3.6\", version=\">=18.9b0\", allow_prereleases=true }\n", 70};

    const Table expected{{ "black", Value::of_table({{ "allow_prereleases", Value::of_boolean(true) },{ "python", Value::of_string(">3.6") },{ "version", Value::of_string(">=18.9b0") }}) }};

    assert_parsed(toml, expected);
}
