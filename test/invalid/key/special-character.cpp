TEST(invalid_key_tests, test_special_character)
{
    const string toml("\316\274 = \"greek small letter mu\"\n", 29);

    const vector<Error> expected = {
        { 0, 1, 1, "Invalid key: μ" },
    };

    assert_errors(toml, expected);
}
