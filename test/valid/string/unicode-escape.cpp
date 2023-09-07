TEST(valid_string_tests, test_unicode_escape)
{
    const string toml{"answer4 = \"\\u03B4\"\nanswer8 = \"\\U000003B4\"\n", 42};

    const Table expected{{ "answer4", Value::of_string("δ") },{ "answer8", Value::of_string("δ") }};

    assert_parsed(toml, expected);
}
