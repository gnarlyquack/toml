TEST(invalid_integer_tests, test_capital_hex)
{
    const string toml{"capital-hex = 0X1\n", 18};

    const vector<Error> expected{{ 1, 15, "Invalid decimal number: 0X1" },};

    assert_errors(toml, expected);
}
