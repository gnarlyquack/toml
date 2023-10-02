TEST(invalid_integer_tests, test_negative_bin)
{
    const string toml("negative-bin = -0b11010110\n", 27);

    const vector<Error> expected = {
        { 15, 1, 16, "Binary number may not have a leading sign." },
    };

    assert_errors(toml, expected);
}
