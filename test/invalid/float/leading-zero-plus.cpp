TEST(invalid_float_tests, test_leading_zero_plus)
{
    const string toml("leading-zero-plus = +03.14\n", 27);

    const vector<Error> expected = {
        { 21, 1, 22, "Number may not have leading zeros." },
    };

    assert_errors(toml, expected);
}
