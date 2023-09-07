TEST(valid_datetime_tests, test_datetime)
{
    const string toml{"space = 1987-07-05 17:45:00Z\nlower = 1987-07-05t17:45:00z\n", 58};

    const Table expected{{ "lower", Value::of_offset_datetime(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ "space", Value::of_offset_datetime(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) }};

    assert_parsed(toml, expected);
}
