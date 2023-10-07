TEST(valid_spec_tests, test_string_5)
{
    const string toml("# What you see is what you get.\nwinpath  = 'C:\\Users\\nodejs\\templates'\nwinpath2 = '\\\\ServerX\\admin$\\system32\\'\nquoted   = 'Tom \"Dubs\" Preston-Werner'\nregex    = '<\\i\\c*\\s*>'\n", 174);

    const Table expected = {{ std::string("quoted", 6), Value(std::string("Tom \"Dubs\" Preston-Werner", 25)) },{ std::string("regex", 5), Value(std::string("<\\i\\c*\\s*>", 10)) },{ std::string("winpath", 7), Value(std::string("C:\\Users\\nodejs\\templates", 25)) },{ std::string("winpath2", 8), Value(std::string("\\\\ServerX\\admin$\\system32\\", 26)) }};

    assert_parsed(toml, expected);
}
