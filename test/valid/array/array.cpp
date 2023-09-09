TEST(valid_array_tests, test_array)
{
    const string toml{"ints = [1, 2, 3, ]\nfloats = [1.1, 2.1, 3.1]\nstrings = [\"a\", \"b\", \"c\"]\ndates = [\n  1987-07-05T17:45:00Z,\n  1979-05-27T07:32:00Z,\n  2006-06-01T11:00:00Z,\n]\ncomments = [\n         1,\n         2, #this is ok\n]\n", 205};

    const Table expected{{ "comments", Value(Array({Value(INT64_C(1)),Value(INT64_C(2))})) },{ "dates", Value(Array({Value(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)),Value(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)),Value(date::sys_days(date::year(2006) / date::month(6) / date::day(1)) + chrono::hours(11) + chrono::minutes(0) + chrono::seconds(0) + chrono::microseconds(0))})) },{ "floats", Value(Array({Value(1.1),Value(2.1),Value(3.1)})) },{ "ints", Value(Array({Value(INT64_C(1)),Value(INT64_C(2)),Value(INT64_C(3))})) },{ "strings", Value(Array({Value(std::string("a")),Value(std::string("b")),Value(std::string("c"))})) }};

    assert_parsed(toml, expected);
}
