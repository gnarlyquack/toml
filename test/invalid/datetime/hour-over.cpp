TEST(invalid_datetime_tests, test_hour_over)
{
    const string toml("# time-hour       = 2DIGIT  ; 00-23\nd = 2006-01-01T24:00:00-00:00\n", 66);

    const vector<Error> expected = {
        { 51, 2, 16, "Hour must be between 00 and 23 inclusive but value was: 24" },
    };

    assert_errors(toml, expected);
}
