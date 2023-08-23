#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_trailing_us_bin)
{
    const string toml{"trailing-us-bin = 0b1_\n", 23};

    const vector<Error> expected{{ 1, 19, "'_' must separate two digits in number: 0b1_." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_us_after_hex)
{
    const string toml{"us-after-hex = 0x_1\n", 20};

    const vector<Error> expected{{ 1, 16, "'_' must separate two digits in number: 0x_1." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_trailing_us)
{
    const string toml{"trailing-us = 123_\n", 19};

    const vector<Error> expected{{ 1, 15, "'_' must separate two digits in number: 123_." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_invalid_hex)
{
    const string toml{"invalid-hex = 0xaafz\n", 21};

    const vector<Error> expected{{ 1, 15, "Invalid value for hexadecimal integer: 0xaafz." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_negative_bin)
{
    const string toml{"negative-bin = -0b11010110\n", 27};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in binary integer: 0b11010110." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_zero_sign_3)
{
    const string toml{"leading-zero-sign-3 = +0_1\n", 27};

    const vector<Error> expected{{ 1, 27, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_us_hex)
{
    const string toml{"leading-us-hex = _0o1\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid value: _0o1." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_double_sign_nex)
{
    const string toml{"double-sign-nex = --99\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid value: -99." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_zero_1)
{
    const string toml{"leading-zero-1 = 01\n", 20};

    const vector<Error> expected{{ 1, 20, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_incomplete_hex)
{
    const string toml{"incomplete-hex = 0x\n", 20};

    const vector<Error> expected{{ 1, 20, "Missing value for hexadecimal integer." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_capital_oct)
{
    const string toml{"capital-oct = 0O0\n", 18};

    const vector<Error> expected{{ 1, 15, "Invalid value for decimal integer: 0O0." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_invalid_bin)
{
    const string toml{"invalid-bin = 0b0012\n", 21};

    const vector<Error> expected{{ 1, 15, "Invalid value for binary integer: 0b0012." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_negative_oct)
{
    const string toml{"negative-oct = -0o99\n", 21};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in octal integer: 0o99." },{ 1, 17, "Invalid value for octal integer: 0o99." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_trailing_us_oct)
{
    const string toml{"trailing-us-oct = 0o1_\n", 23};

    const vector<Error> expected{{ 1, 19, "'_' must separate two digits in number: 0o1_." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_negative_hex)
{
    const string toml{"negative-hex = -0xff\n", 21};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in hexadecimal integer: 0xff." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_capital_bin)
{
    const string toml{"capital-bin = 0B0\n", 18};

    const vector<Error> expected{{ 1, 15, "Invalid value for decimal integer: 0B0." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_zero_2)
{
    const string toml{"leading-zero-2 = 00\n", 20};

    const vector<Error> expected{{ 1, 20, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_us_bin)
{
    const string toml{"leading-us-bin = _0o1\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid value: _0o1." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_positive_oct)
{
    const string toml{"positive-oct = +0o99\n", 21};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in octal integer: 0o99." },{ 1, 17, "Invalid value for octal integer: 0o99." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_capital_hex)
{
    const string toml{"capital-hex = 0X1\n", 18};

    const vector<Error> expected{{ 1, 15, "Invalid value for decimal integer: 0X1." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_zero_3)
{
    const string toml{"leading-zero-3 = 0_0\n", 21};

    const vector<Error> expected{{ 1, 21, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_zero_sign_1)
{
    const string toml{"leading-zero-sign-1 = -01\n", 26};

    const vector<Error> expected{{ 1, 26, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_us)
{
    const string toml{"leading-us = _123\n", 18};

    const vector<Error> expected{{ 1, 14, "Invalid value: _123." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_zero_sign_2)
{
    const string toml{"leading-zero-sign-2 = +01\n", 26};

    const vector<Error> expected{{ 1, 26, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_positive_bin)
{
    const string toml{"positive-bin = +0b11010110\n", 27};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in binary integer: 0b11010110." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_trailing_us_hex)
{
    const string toml{"trailing-us-hex = 0x1_\n", 23};

    const vector<Error> expected{{ 1, 19, "'_' must separate two digits in number: 0x1_." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_us_after_oct)
{
    const string toml{"us-after-oct = 0o_1\n", 20};

    const vector<Error> expected{{ 1, 16, "'_' must separate two digits in number: 0o_1." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_text_after_integer)
{
    const string toml{"answer = 42 the ultimate answer?\n", 33};

    const vector<Error> expected{{ 1, 13, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_leading_us_oct)
{
    const string toml{"leading-us-oct = _0o1\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid value: _0o1." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_positive_hex)
{
    const string toml{"positive-hex = +0xff\n", 21};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in hexadecimal integer: 0xff." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_incomplete_bin)
{
    const string toml{"incomplete-bin = 0b\n", 20};

    const vector<Error> expected{{ 1, 20, "Missing value for binary integer." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_invalid_oct)
{
    const string toml{"invalid-oct = 0o778\n", 20};

    const vector<Error> expected{{ 1, 15, "Invalid value for octal integer: 0o778." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_us_after_bin)
{
    const string toml{"us-after-bin = 0b_1\n", 20};

    const vector<Error> expected{{ 1, 16, "'_' must separate two digits in number: 0b_1." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_incomplete_oct)
{
    const string toml{"incomplete-oct = 0o\n", 20};

    const vector<Error> expected{{ 1, 20, "Missing value for octal integer." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_double_sign_plus)
{
    const string toml{"double-sign-plus = ++99\n", 24};

    const vector<Error> expected{{ 1, 21, "Invalid value: +99." },};

    assert_errors(toml, expected);
}


TEST(invalid_integer_tests, test_double_us)
{
    const string toml{"double-us = 1__23\n", 18};

    const vector<Error> expected{{ 1, 13, "'_' must separate two digits in number: 1__23." },};

    assert_errors(toml, expected);
}
