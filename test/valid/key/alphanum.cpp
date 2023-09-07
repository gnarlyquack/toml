TEST(valid_key_tests, test_alphanum)
{
    const string toml{"alpha = \"a\"\n123 = \"num\"\n000111 = \"leading\"\n10e3 = \"false float\"\none1two2 = \"mixed\"\nwith-dash = \"dashed\"\nunder_score = \"___\"\n34-11 = 23\n\n[2018_10]\n001 = 1\n\n[a-a-a]\n_ = false\n", 173};

    const Table expected{{ "000111", Value::of_string("leading") },{ "10e3", Value::of_string("false float") },{ "123", Value::of_string("num") },{ "2018_10", Value::of_table({{ "001", Value::of_integer(1ULL) }}) },{ "34-11", Value::of_integer(23ULL) },{ "a-a-a", Value::of_table({{ "_", Value::of_boolean(false) }}) },{ "alpha", Value::of_string("a") },{ "one1two2", Value::of_string("mixed") },{ "under_score", Value::of_string("___") },{ "with-dash", Value::of_string("dashed") }};

    assert_parsed(toml, expected);
}
