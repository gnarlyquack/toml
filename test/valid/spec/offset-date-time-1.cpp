TEST(valid_spec_tests, test_offset_date_time_1)
{
    const string toml("odt4 = 1979-05-27 07:32:00Z\n", 28);

    const Table expected = {{ std::string("odt4", 4), Value(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) }};

    assert_parsed(toml, expected);
}
