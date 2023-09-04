TEST(invalid_float_tests, test_double_point_2)
{
    const string toml{"double-point-2 = 0.1.2\n", 23};

    const vector<Error> expected{{ 1, 20, "Invalid fractional part of decimal number: 0.1.2" },};

    assert_errors(toml, expected);
}
