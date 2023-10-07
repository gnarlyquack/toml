TEST(valid_spec_tests, test_local_time_0)
{
    const string toml("lt1 = 07:32:00\nlt2 = 00:32:00.999999\n", 37);

    const Table expected = {{ std::string("lt1", 3), Value(LocalTime(chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0))) },{ std::string("lt2", 3), Value(LocalTime(chrono::hours(0) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(999999))) }};

    assert_parsed(toml, expected);
}
