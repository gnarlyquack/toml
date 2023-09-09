TEST(valid_datetime_tests, test_timezone)
{
    const string toml{"utc  = 1987-07-05T17:45:56Z\npdt  = 1987-07-05T17:45:56-05:00\nnzst = 1987-07-05T17:45:56+12:00\nnzdt = 1987-07-05T17:45:56+13:00  # DST\n", 134};

    const Table expected{{ "nzdt", Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(4) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) },{ "nzst", Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(5) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) },{ "pdt", Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(22) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) },{ "utc", Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) }};

    assert_parsed(toml, expected);
}
