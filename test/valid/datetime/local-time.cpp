TEST(valid_datetime_tests, test_local_time)
{
    const string toml("besttimeever = 17:45:00\nmilliseconds = 10:32:00.555\n", 52);

    const Table expected = {{ std::string("besttimeever", 12), Value(LocalTime(chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0))) },{ std::string("milliseconds", 12), Value(LocalTime(chrono::hours(10) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(555000))) }};

    assert_parsed(toml, expected);
}
