TEST(valid_datetime_tests, test_local)
{
    const string toml("local = 1987-07-05T17:45:00\nmilli = 1977-12-21T10:32:00.555\nspace = 1987-07-05 17:45:00\n", 88);

    const Table expected = {{ std::string("local", 5), Value(date::local_time<date::days>(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ std::string("milli", 5), Value(date::local_time<date::days>(date::year(1977) / date::month(12) / date::day(21)) + chrono::hours(10) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(555000)) },{ std::string("space", 5), Value(date::local_time<date::days>(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) }};

    assert_parsed(toml, expected);
}
