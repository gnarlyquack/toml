TEST(invalid_string_tests, test_bad_concat)
{
    const string toml("no_concat = \"first\" \"second\"\n", 29);

    const vector<Error> expected = {
        { 20, 1, 21, "Expected the end of the line but got: \"second\"" },
    };

    assert_errors(toml, expected);
}
