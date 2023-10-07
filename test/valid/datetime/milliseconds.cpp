TEST(valid_datetime_tests, test_milliseconds)
{
    const string toml("utc1  = 1987-07-05T17:45:56.123Z\nutc2  = 1987-07-05T17:45:56.6Z\nwita1 = 1987-07-05T17:45:56.123+08:00\nwita2 = 1987-07-05T17:45:56.6+08:00\n", 138);

    const Table expected = {{ std::string("utc1", 4), Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(123000)) },{ std::string("utc2", 4), Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(600000)) },{ std::string("wita1", 5), Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(9) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(123000)) },{ std::string("wita2", 5), Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(9) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(600000)) }};

    assert_parsed(toml, expected);
}
