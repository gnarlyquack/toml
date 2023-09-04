TEST(invalid_key_tests, test_after_table)
{
    const string toml{"[error] this = \"should not be here\"\n", 36};

    const vector<Error> expected{{ 1, 9, "Expected the end of the line but got: this = \"should not be here\"" },};

    assert_errors(toml, expected);
}
