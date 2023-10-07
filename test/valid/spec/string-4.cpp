TEST(valid_spec_tests, test_string_4)
{
    const string toml("str4 = \"\"\"Here are two quotation marks: \"\". Simple enough.\"\"\"\n# str5 = \"\"\"Here are three quotation marks: \"\"\".\"\"\"  # INVALID\nstr5 = \"\"\"Here are three quotation marks: \"\"\\\".\"\"\"\nstr6 = \"\"\"Here are fifteen quotation marks: \"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\"\"\"\\\".\"\"\"\n\n# \"This,\" she said, \"is just a pointless statement.\"\nstr7 = \"\"\"\"This,\" she said, \"is just a pointless statement.\"\"\"\"\n", 363);

    const Table expected = {{ std::string("str4", 4), Value(std::string("Here are two quotation marks: \"\". Simple enough.", 48)) },{ std::string("str5", 4), Value(std::string("Here are three quotation marks: \"\"\".", 36)) },{ std::string("str6", 4), Value(std::string("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\".", 50)) },{ std::string("str7", 4), Value(std::string("\"This,\" she said, \"is just a pointless statement.\"", 50)) }};

    assert_parsed(toml, expected);
}
