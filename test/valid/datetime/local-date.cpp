TEST(valid_datetime_tests, test_local_date)
{
    const string toml("bestdayever = 1987-07-05\n", 25);

    const Table expected = {{ std::string("bestdayever", 11), Value(LocalDate(date::year(1987) / date::month(7) / date::day(5))) }};

    assert_parsed(toml, expected);
}
