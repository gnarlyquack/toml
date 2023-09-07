TEST(valid_array_tests, test_strings)
{
    const string toml{"string_array = [ \"all\", 'strings', \"\"\"are the same\"\"\", '''type''']\n", 67};

    const Table expected{{ "string_array", Value::of_array({Value::of_string("all"),Value::of_string("strings"),Value::of_string("are the same"),Value::of_string("type")}) }};

    assert_parsed(toml, expected);
}
