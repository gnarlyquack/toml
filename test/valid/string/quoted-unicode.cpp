TEST(valid_string_tests, test_quoted_unicode)
{
    const string toml("\nescaped_string = \"\\u0000 \\u0008 \\u000c \\U00000041 \\u007f \\u0080 \\u00ff \\ud7ff \\ue000 \\uffff \\U00010000 \\U0010ffff\"\nnot_escaped_string = '\\u0000 \\u0008 \\u000c \\U00000041 \\u007f \\u0080 \\u00ff \\ud7ff \\ue000 \\uffff \\U00010000 \\U0010ffff'\n\nbasic_string = \"~ \302\200 \303\277 \355\237\277 \356\200\200 \357\277\277 \360\220\200\200 \364\217\277\277\"\nliteral_string = '~ \302\200 \303\277 \355\237\277 \356\200\200 \357\277\277 \360\220\200\200 \364\217\277\277'\n", 332);

    const Table expected = {{ std::string("escaped_string", 14), Value(std::string("\u0000 \b \f A \u007F \u0080 ÿ \uD7FF \uE000 \uFFFF 𐀀 \U0010FFFF""", 37)) },{ std::string("not_escaped_string", 18), Value(std::string("\\u0000 \\u0008 \\u000c \\U00000041 \\u007f \\u0080 \\u00ff \\ud7ff \\ue000 \\uffff \\U00010000 \\U0010ffff", 95)) },{ std::string("basic_string", 12), Value(std::string("~ \u0080 ÿ \uD7FF \uE000 \uFFFF 𐀀 \U0010FFFF""", 29)) },{ std::string("literal_string", 14), Value(std::string("~ \u0080 ÿ \uD7FF \uE000 \uFFFF 𐀀 \U0010FFFF""", 29)) }};

    assert_parsed(toml, expected);
}
