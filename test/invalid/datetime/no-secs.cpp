TEST(invalid_datetime_tests, test_no_secs)
{
    const string toml("# No seconds in time.\nno-secs = 1987-07-05T17:45Z\n", 50);

    const vector<Error> expected = {
        { 48, 2, 27, "Invalid or missing seconds for time." },
    };

    assert_errors(toml, expected);
}
