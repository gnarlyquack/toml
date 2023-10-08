TEST(invalid_encoding_tests, test_utf16)
{
    const string toml("\000#\000 \000U\000T\000F\000-\0001\0006\000 \000w\000i\000t\000h\000o\000u\000t\000 \000B\000O\000M\000\n", 42);

    const vector<Error> expected = {
        {  0, 1,  1, "Detected invalid file encoding. TOML files must be Unicode documents encoded in UTF-8." },
    };

    assert_errors(toml, expected);
}
