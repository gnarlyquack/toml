TEST(valid_spec_tests, test_local_date_time_0)
{
    const string toml("ldt1 = 1979-05-27T07:32:00\nldt2 = 1979-05-27T00:32:00.999999\n", 61);

    const Table expected = {{ std::string("ldt1", 4), Value(date::local_time<date::days>(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) },{ std::string("ldt2", 4), Value(date::local_time<date::days>(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(0) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(999999)) }};

    assert_parsed(toml, expected);
}
