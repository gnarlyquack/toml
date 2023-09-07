TEST(valid_string_tests, test_simple)
{
    const string toml{"answer = \"You are not drinking enough whisky.\"\n", 47};

    const Table expected{{ "answer", Value::of_string("You are not drinking enough whisky.") }};

    assert_parsed(toml, expected);
}
