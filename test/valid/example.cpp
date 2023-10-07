TEST(valid_tests, test_example)
{
    const string toml("best-day-ever = 1987-07-05T17:45:00Z\n\n[numtheory]\nboring = false\nperfection = [6, 28, 496]\n", 91);

    const Table expected = {{ std::string("best-day-ever", 13), Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ std::string("numtheory", 9), Value(Table({{ std::string("boring", 6), Value(false) },{ std::string("perfection", 10), Value(Array({Value(INT64_C(6)),Value(INT64_C(28)),Value(INT64_C(496))})) }})) }};

    assert_parsed(toml, expected);
}
