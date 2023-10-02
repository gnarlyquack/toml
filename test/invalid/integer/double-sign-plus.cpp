TEST(invalid_integer_tests, test_double_sign_plus)
{
    const string toml("double-sign-plus = ++99\n", 24);

    const vector<Error> expected = {
        { 19, 1, 20, "Invalid value: ++99" },
    };

    assert_errors(toml, expected);
}
