TEST(invalid_datetime_tests, test_mday_under)
{
    const string toml("# date-mday       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on\n#                           ; month/year\nd = 2006-01-00T00:00:00-00:00\n", 137);

    const vector<Error> expected = {
        { 119, 3, 13, "Day must be between 01 and 31 inclusive." },
    };

    assert_errors(toml, expected);
}
