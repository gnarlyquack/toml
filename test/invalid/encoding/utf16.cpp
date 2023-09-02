#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_encoding_tests, test_utf16)
{
    const string toml{"\000#\000 \000U\000T\000F\000-\0001\0006\000 \000w\000i\000t\000h\000o\000u\000t\000 \000B\000O\000M\000\n", 42};

    const vector<Error> expected{{ 1, 1, "Unicode codepoint U+0000 is not allowed." },{ 1, 2, "Missing '=' between key and value." },{ 1, 2, "Missing value." },{ 1, 3, "Unicode codepoint U+0000 is not allowed." },{ 1, 5, "Unicode codepoint U+0000 is not allowed." },{ 1, 7, "Unicode codepoint U+0000 is not allowed." },{ 1, 9, "Unicode codepoint U+0000 is not allowed." },{ 1, 11, "Unicode codepoint U+0000 is not allowed." },{ 1, 13, "Unicode codepoint U+0000 is not allowed." },{ 1, 15, "Unicode codepoint U+0000 is not allowed." },{ 1, 17, "Unicode codepoint U+0000 is not allowed." },{ 1, 19, "Unicode codepoint U+0000 is not allowed." },{ 1, 21, "Unicode codepoint U+0000 is not allowed." },{ 1, 23, "Unicode codepoint U+0000 is not allowed." },{ 1, 25, "Unicode codepoint U+0000 is not allowed." },{ 1, 27, "Unicode codepoint U+0000 is not allowed." },{ 1, 29, "Unicode codepoint U+0000 is not allowed." },{ 1, 31, "Unicode codepoint U+0000 is not allowed." },{ 1, 33, "Unicode codepoint U+0000 is not allowed." },{ 1, 35, "Unicode codepoint U+0000 is not allowed." },{ 1, 37, "Unicode codepoint U+0000 is not allowed." },{ 1, 39, "Unicode codepoint U+0000 is not allowed." },{ 1, 41, "Unicode codepoint U+0000 is not allowed." },};

    assert_errors(toml, expected);
}
