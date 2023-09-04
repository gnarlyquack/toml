TEST(invalid_integer_tests, test_us_after_bin)
{
    const string toml{"us-after-bin = 0b_1\n", 20};

    const vector<Error> expected{{ 1, 18, "'_' must separate digits in binary number: 0b_1" },};

    assert_errors(toml, expected);
}
