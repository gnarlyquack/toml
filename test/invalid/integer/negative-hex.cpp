TEST(invalid_integer_tests, test_negative_hex)
{
    const string toml("negative-hex = -0xff\n", 21);

    const vector<Error> expected = {
        { 15, 1, 16, "Hexadecimal number may not have a leading sign." },
    };

    assert_errors(toml, expected);
}
