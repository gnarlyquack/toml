TEST(valid_array_tests, test_strings)
{
    const string toml{"string_array = [ \"all\", 'strings', \"\"\"are the same\"\"\", '''type''']\n", 67};

    const Table expected{{ "string_array", new ArrayValue({new StringValue("all"),new StringValue("strings"),new StringValue("are the same"),new StringValue("type"),}) },};

    assert_parsed(toml, expected);
}
