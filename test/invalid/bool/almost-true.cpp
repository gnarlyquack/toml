TEST(invalid_bool_tests, test_almost_true)
{
    const string toml{"a = tru\n", 8};

    const vector<Error> expected{{ 1, 5, "Invalid value: tru" },};

    assert_errors(toml, expected);
}
