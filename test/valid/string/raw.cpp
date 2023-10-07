TEST(valid_string_tests, test_raw)
{
    const string toml("backspace = 'This string has a \\b backspace character.'\ntab = 'This string has a \\t tab character.'\nunescaped_tab = 'This string has an \t unescaped tab character.'\nnewline = 'This string has a \\n new line character.'\nformfeed = 'This string has a \\f form feed character.'\ncarriage = 'This string has a \\r carriage return character.'\nslash = 'This string has a \\/ slash character.'\nbackslash = 'This string has a \\\\ backslash character.'\n", 437);

    const Table expected = {{ std::string("backslash", 9), Value(std::string("This string has a \\\\ backslash character.", 41)) },{ std::string("backspace", 9), Value(std::string("This string has a \\b backspace character.", 41)) },{ std::string("carriage", 8), Value(std::string("This string has a \\r carriage return character.", 47)) },{ std::string("formfeed", 8), Value(std::string("This string has a \\f form feed character.", 41)) },{ std::string("newline", 7), Value(std::string("This string has a \\n new line character.", 40)) },{ std::string("slash", 5), Value(std::string("This string has a \\/ slash character.", 37)) },{ std::string("tab", 3), Value(std::string("This string has a \\t tab character.", 35)) },{ std::string("unescaped_tab", 13), Value(std::string("This string has an \t unescaped tab character.", 45)) }};

    assert_parsed(toml, expected);
}
