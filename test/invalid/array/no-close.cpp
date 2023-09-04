TEST(invalid_array_tests, test_no_close)
{
    const string toml{"long_array = [ 1, 2, 3\n", 23};

    const vector<Error> expected{{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}
