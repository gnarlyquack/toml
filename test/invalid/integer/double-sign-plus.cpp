TEST(invalid_integer_tests, test_double_sign_plus)
{
    const string toml{"double-sign-plus = ++99\n", 24};

    const vector<Error> expected{{ 1, 21, "Invalid decimal number: ++99" },};

    assert_errors(toml, expected);
}
