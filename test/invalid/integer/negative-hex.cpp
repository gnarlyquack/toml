TEST(invalid_integer_tests, test_negative_hex)
{
    const string toml("negative-hex = -0xff\n", 21);

    const vector<Error> expected = {
        { 15, 1, 16, "A leading sign is not allowed." },
    };

    assert_errors(toml, expected);
}
