#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_float_tests, test_leading_point_neg)
{
    const string toml{"leading-point-neg = -.12345\n", 28};

    const vector<Error> expected{{ 1, 22, "Missing value for whole part of decimal number." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_nan_incomplete_1)
{
    const string toml{"nan-incomplete-1 = na\n", 22};

    const vector<Error> expected{{ 1, 20, "Invalid floating point value." },{ 1, 20, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_exp_trailing_us)
{
    const string toml{"exp-trailing-us = 1e_23_\n", 25};

    const vector<Error> expected{{ 1, 21, "'_' must separate two digits in number: _23_." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_trailing_us)
{
    const string toml{"trailing-us = 1.2_\n", 19};

    const vector<Error> expected{{ 1, 17, "'_' must separate two digits in number: 2_." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_exp_double_e_2)
{
    const string toml{"exp-double-e-2 = 1e2e3\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid value for exponential part of decimal number: 2e3." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_leading_zero_plus)
{
    const string toml{"leading-zero-plus = +03.14\n", 27};

    const vector<Error> expected{{ 1, 24, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_exp_point_2)
{
    const string toml{"exp-point-2 = 1.e2\n", 19};

    const vector<Error> expected{{ 1, 17, "Missing value for fractional part of decimal number." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_trailing_point_plus)
{
    const string toml{"trailing-point-plus = +1.\n", 26};

    const vector<Error> expected{{ 1, 26, "Missing value for fractional part of decimal number." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_double_point_2)
{
    const string toml{"double-point-2 = 0.1.2\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid value for fractional part of decimal number: 1.2." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_inf_incomplete_1)
{
    const string toml{"inf-incomplete-1 = in\n", 22};

    const vector<Error> expected{{ 1, 20, "Invalid floating point value." },{ 1, 20, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_exp_leading_us)
{
    const string toml{"exp-leading-us = 1e_23\n", 23};

    const vector<Error> expected{{ 1, 20, "'_' must separate two digits in number: _23." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_us_after_point)
{
    const string toml{"us-after-point = 1._2\n", 22};

    const vector<Error> expected{{ 1, 20, "'_' must separate two digits in number: _2." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_leading_point)
{
    const string toml{"leading-point = .12345\n", 23};

    const vector<Error> expected{{ 1, 17, "Missing value for whole part of decimal number." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_trailing_point_min)
{
    const string toml{"trailing-point-min = -1.\n", 25};

    const vector<Error> expected{{ 1, 25, "Missing value for fractional part of decimal number." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_exp_double_e_1)
{
    const string toml{"exp-double-e-1 = 1ee2\n", 22};

    const vector<Error> expected{{ 1, 20, "Invalid value for exponential part of decimal number: e2." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_nan_incomplete_2)
{
    const string toml{"nan-incomplete-2 = +na\n", 23};

    const vector<Error> expected{{ 1, 21, "Invalid floating point value." },{ 1, 21, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_inf_incomplete_2)
{
    const string toml{"inf-incomplete-2 = +in\n", 23};

    const vector<Error> expected{{ 1, 21, "Invalid floating point value." },{ 1, 21, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_exp_point_1)
{
    const string toml{"exp-point-1 = 1e2.3\n", 20};

    const vector<Error> expected{{ 1, 17, "Invalid value for exponential part of decimal number: 2.3." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_inf_underscore)
{
    const string toml{"inf_underscore = in_f\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid floating point value." },{ 1, 18, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_leading_us)
{
    const string toml{"leading-us = _1.2\n", 18};

    const vector<Error> expected{{ 1, 14, "Invalid value: _1.2." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_double_point_1)
{
    const string toml{"double-point-1 = 0..1\n", 22};

    const vector<Error> expected{{ 1, 20, "Invalid value for fractional part of decimal number: .1." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_trailing_us_exp)
{
    const string toml{"# trailing underscore in integer part is not allowed\ntrailing-us-exp = 1_e2\n# trailing underscore in float part is not allowed\ntrailing-us-exp2 = 1.2_e2\n", 153};

    const vector<Error> expected{{ 2, 19, "'_' must separate two digits in number: 1_." },{ 4, 22, "'_' must separate two digits in number: 2_." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_inf_incomplete_3)
{
    const string toml{"inf-incomplete-3 = -in\n", 23};

    const vector<Error> expected{{ 1, 21, "Invalid floating point value." },{ 1, 21, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_leading_zero)
{
    const string toml{"leading-zero = 03.14\n", 21};

    const vector<Error> expected{{ 1, 18, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_us_before_point)
{
    const string toml{"us-before-point = 1_.2\n", 23};

    const vector<Error> expected{{ 1, 19, "'_' must separate two digits in number: 1_." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_leading_point_plus)
{
    const string toml{"leading-point-plus = +.12345\n", 29};

    const vector<Error> expected{{ 1, 23, "Missing value for whole part of decimal number." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_trailing_point)
{
    const string toml{"trailing-point = 1.\n", 20};

    const vector<Error> expected{{ 1, 20, "Missing value for fractional part of decimal number." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_nan_incomplete_3)
{
    const string toml{"nan-incomplete-3 = -na\n", 23};

    const vector<Error> expected{{ 1, 21, "Invalid floating point value." },{ 1, 21, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_nan_underscore)
{
    const string toml{"nan_underscore = na_n\n", 22};

    const vector<Error> expected{{ 1, 18, "Invalid floating point value." },{ 1, 18, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_exp_double_us)
{
    const string toml{"exp-double-us = 1e__23\n", 23};

    const vector<Error> expected{{ 1, 19, "'_' must separate two digits in number: __23." },};

    assert_errors(toml, expected);
}


TEST(invalid_float_tests, test_leading_zero_neg)
{
    const string toml{"leading-zero-neg = -03.14\n", 26};

    const vector<Error> expected{{ 1, 23, "Leading zeros in decimal number not allowed." },};

    assert_errors(toml, expected);
}
