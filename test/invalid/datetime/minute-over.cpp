TEST(invalid_datetime_tests, test_minute_over)
{
    const string toml("# time-minute     = 2DIGIT  ; 00-59\nd = 2006-01-01T00:60:00-00:00\n", 66);

    const vector<Error> expected = {
        { 54, 2, 19, "Minute must be between 00 and 59 inclusive but value was: 60" },
    };

    assert_errors(toml, expected);
}
