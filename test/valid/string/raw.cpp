TEST(valid_string_tests, test_raw)
{
    const string toml{"backspace = 'This string has a \\b backspace character.'\ntab = 'This string has a \\t tab character.'\nunescaped_tab = 'This string has an \t unescaped tab character.'\nnewline = 'This string has a \\n new line character.'\nformfeed = 'This string has a \\f form feed character.'\ncarriage = 'This string has a \\r carriage return character.'\nslash = 'This string has a \\/ slash character.'\nbackslash = 'This string has a \\\\ backslash character.'\n", 437};

    const Table expected{{ "backslash", Value(std::string("This string has a \\\\ backslash character.")) },{ "backspace", Value(std::string("This string has a \\b backspace character.")) },{ "carriage", Value(std::string("This string has a \\r carriage return character.")) },{ "formfeed", Value(std::string("This string has a \\f form feed character.")) },{ "newline", Value(std::string("This string has a \\n new line character.")) },{ "slash", Value(std::string("This string has a \\/ slash character.")) },{ "tab", Value(std::string("This string has a \\t tab character.")) },{ "unescaped_tab", Value(std::string("This string has an \t unescaped tab character.")) }};

    assert_parsed(toml, expected);
}
