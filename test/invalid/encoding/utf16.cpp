TEST(invalid_encoding_tests, test_utf16)
{
    const string toml("\000#\000 \000U\000T\000F\000-\0001\0006\000 \000w\000i\000t\000h\000o\000u\000t\000 \000B\000O\000M\000\n", 42);

    const vector<Error> expected = {
        {  0, 1,  1, "Unicode codepoint U+0000 is not allowed." },
        {  1, 1,  2, "Missing '=' between key and value." },
        {  1, 1,  2, "Expected a value." },
        {  2, 1,  3, "Unicode codepoint U+0000 is not allowed." },
        {  4, 1,  5, "Unicode codepoint U+0000 is not allowed." },
        {  6, 1,  7, "Unicode codepoint U+0000 is not allowed." },
        {  8, 1,  9, "Unicode codepoint U+0000 is not allowed." },
        { 10, 1, 11, "Unicode codepoint U+0000 is not allowed." },
        { 12, 1, 13, "Unicode codepoint U+0000 is not allowed." },
        { 14, 1, 15, "Unicode codepoint U+0000 is not allowed." },
        { 16, 1, 17, "Unicode codepoint U+0000 is not allowed." },
        { 18, 1, 19, "Unicode codepoint U+0000 is not allowed." },
        { 20, 1, 21, "Unicode codepoint U+0000 is not allowed." },
        { 22, 1, 23, "Unicode codepoint U+0000 is not allowed." },
        { 24, 1, 25, "Unicode codepoint U+0000 is not allowed." },
        { 26, 1, 27, "Unicode codepoint U+0000 is not allowed." },
        { 28, 1, 29, "Unicode codepoint U+0000 is not allowed." },
        { 30, 1, 31, "Unicode codepoint U+0000 is not allowed." },
        { 32, 1, 33, "Unicode codepoint U+0000 is not allowed." },
        { 34, 1, 35, "Unicode codepoint U+0000 is not allowed." },
        { 36, 1, 37, "Unicode codepoint U+0000 is not allowed." },
        { 38, 1, 39, "Unicode codepoint U+0000 is not allowed." },
        { 40, 1, 41, "Unicode codepoint U+0000 is not allowed." },
    };

    assert_errors(toml, expected);
}
