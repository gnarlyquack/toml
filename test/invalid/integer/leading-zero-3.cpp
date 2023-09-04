TEST(invalid_integer_tests, test_leading_zero_3)
{
    const string toml{"leading-zero-3 = 0_0\n", 21};

    const vector<Error> expected{{ 1, 18, "Leading zeros are not allowed in decimal number." },};

    assert_errors(toml, expected);
}
