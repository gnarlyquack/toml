TEST(invalid_key_tests, test_empty)
{
    const string toml{" = 1\n", 5};

    const vector<Error> expected{{ 1, 2, "Missing key." },};

    assert_errors(toml, expected);
}
