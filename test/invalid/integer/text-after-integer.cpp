TEST(invalid_integer_tests, test_text_after_integer)
{
    const string toml("answer = 42 the ultimate answer?\n", 33);

    const vector<Error> expected = {
        { 12, 1, 13, "Expected the end of the line but got: the ultimate answer?" },
    };

    assert_errors(toml, expected);
}
