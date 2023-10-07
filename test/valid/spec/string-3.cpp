TEST(valid_spec_tests, test_string_3)
{
    const string toml("# The following strings are byte-for-byte equivalent:\nstr1 = \"The quick brown fox jumps over the lazy dog.\"\n\nstr2 = \"\"\"\nThe quick brown \\\n\n\n  fox jumps over \\\n    the lazy dog.\"\"\"\n\nstr3 = \"\"\"\\\n       The quick brown \\\n       fox jumps over \\\n       the lazy dog.\\\n       \"\"\"\n", 275);

    const Table expected = {{ std::string("str1", 4), Value(std::string("The quick brown fox jumps over the lazy dog.", 44)) },{ std::string("str2", 4), Value(std::string("The quick brown fox jumps over the lazy dog.", 44)) },{ std::string("str3", 4), Value(std::string("The quick brown fox jumps over the lazy dog.", 44)) }};

    assert_parsed(toml, expected);
}
