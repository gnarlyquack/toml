TEST(invalid_string_tests, test_wrong_close)
{
    const string toml("bad-ending-quote = \"double and single'\n", 39);

    const vector<Error> expected = {
        { 38, 1, 39, "Unclosed string." },
    };

    assert_errors(toml, expected);
}
