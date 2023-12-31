TEST(invalid_array_tests, test_no_close_2)
{
    const string toml("x = [42 #\n", 10);

    const vector<Error> expected = {
        { 10, 2, 1, "Missing closing ']' for array." },
    };

    assert_errors(toml, expected);
}
