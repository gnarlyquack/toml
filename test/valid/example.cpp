TEST(valid_tests, test_example)
{
    const string toml{"best-day-ever = 1987-07-05T17:45:00Z\n\n[numtheory]\nboring = false\nperfection = [6, 28, 496]\n", 91};

    const Table expected{{ "best-day-ever", Value::of_offset_datetime(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ "numtheory", Value::of_table({{ "boring", Value::of_boolean(false) },{ "perfection", Value::of_array({Value::of_integer(6ULL),Value::of_integer(28ULL),Value::of_integer(496ULL)}) }}) }};

    assert_parsed(toml, expected);
}
