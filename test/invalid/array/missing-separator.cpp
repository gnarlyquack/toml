TEST(invalid_array_tests, test_missing_separator)
{
    const string toml("wrong = [ 1 2 3 ]\n", 18);

    const vector<Error> expected = {
        { 12, 1, 13, "Missing ',' between array values." },
        { 14, 1, 15, "Missing ',' between array values." },
    };

    assert_errors(toml, expected);
}
