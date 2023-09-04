TEST(invalid_key_tests, test_single_open_bracket)
{
    const string toml{"[\n", 2};

    const vector<Error> expected{{ 1, 2, "Missing key." },{ 1, 2, "Missing closing ']' for table header." },};

    assert_errors(toml, expected);
}
