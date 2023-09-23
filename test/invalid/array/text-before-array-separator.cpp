TEST(invalid_array_tests, test_text_before_array_separator)
{
    const string toml("array = [\n  \"Is there life before an array separator?\" No,\n  \"Entry\"\n]\n", 71);

    const vector<Error> expected = {
        { 55, 2, 46, "Invalid value: No" },
        { 55, 2, 46, "Missing ',' between array values." },
    };

    assert_errors(toml, expected);
}
