TEST(invalid_integer_tests, test_double_sign_nex)
{
    const string toml("double-sign-nex = --99\n", 23);

    const vector<Error> expected = {
        { 18, 1, 19, "Invalid value." },
    };

    assert_errors(toml, expected);
}
