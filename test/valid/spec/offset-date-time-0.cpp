TEST(valid_spec_tests, test_offset_date_time_0)
{
    const string toml("odt1 = 1979-05-27T07:32:00Z\nodt2 = 1979-05-27T00:32:00-07:00\nodt3 = 1979-05-27T00:32:00.999999-07:00\n", 101);

    const Table expected = {{ std::string("odt1", 4), Value(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) },{ std::string("odt2", 4), Value(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) },{ std::string("odt3", 4), Value(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(999999)) }};

    assert_parsed(toml, expected);
}
