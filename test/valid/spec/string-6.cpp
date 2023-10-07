TEST(valid_spec_tests, test_string_6)
{
    const string toml("regex2 = '''I [dw]on't need \\d{2} apples'''\nlines  = '''\nThe first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n'''\n", 147);

    const Table expected = {{ std::string("lines", 5), Value(std::string("The first newline is\ntrimmed in raw strings.\n   All other whitespace\n   is preserved.\n", 86)) },{ std::string("regex2", 6), Value(std::string("I [dw]on't need \\d{2} apples", 28)) }};

    assert_parsed(toml, expected);
}
