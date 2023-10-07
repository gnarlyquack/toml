TEST(valid_spec_tests, test_string_2)
{
    const string toml("# On a Unix system, the above multi-line string will most likely be the same as:\nstr2 = \"Roses are red\\nViolets are blue\"\n\n# On a Windows system, it will most likely be equivalent to:\nstr3 = \"Roses are red\\r\\nViolets are blue\"\n", 227);

    const Table expected = {{ std::string("str2", 4), Value(std::string("Roses are red\nViolets are blue", 30)) },{ std::string("str3", 4), Value(std::string("Roses are red\r\nViolets are blue", 31)) }};

    assert_parsed(toml, expected);
}
