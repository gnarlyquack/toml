TEST(valid_key_tests, test_quoted_unicode)
{
    const string toml("\n\"\\u0000\" = \"null\"\n'\\u0000' = \"different key\"\n\"\\u0008 \\u000c \\U00000041 \\u007f \\u0080 \\u00ff \\ud7ff \\ue000 \\uffff \\U00010000 \\U0010ffff\" = \"escaped key\"\n\n\"~ \302\200 \303\277 \355\237\277 \356\200\200 \357\277\277 \360\220\200\200 \364\217\277\277\" = \"basic key\"\n'l ~ \302\200 \303\277 \355\237\277 \356\200\200 \357\277\277 \360\220\200\200 \364\217\277\277' = \"literal key\"\n", 250);

    const Table expected = {{ std::string("\u0000", 1), Value(std::string("null", 4)) },{ std::string("\\u0000", 6), Value(std::string("different key", 13)) },{ std::string("\b \f A \u007F \u0080 ÿ \uD7FF \uE000 \uFFFF 𐀀 \U0010FFFF""", 35), Value(std::string("escaped key", 11)) },{ std::string("~ \u0080 ÿ \uD7FF \uE000 \uFFFF 𐀀 \U0010FFFF""", 29), Value(std::string("basic key", 9)) },{ std::string("l ~ \u0080 ÿ \uD7FF \uE000 \uFFFF 𐀀 \U0010FFFF""", 31), Value(std::string("literal key", 11)) }};

    assert_parsed(toml, expected);
}
