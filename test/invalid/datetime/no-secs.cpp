TEST(invalid_datetime_tests, test_no_secs)
{
    const string toml("# No seconds in time.\nno-secs = 1987-07-05T17:45Z\n", 50);

    const vector<Error> expected = {
        // TODO Specifically flag missing/invalid seconds?
        { 32, 2, 11, "Invalid value: 1987-07-05T17:45Z" },
    };

    assert_errors(toml, expected);
}
