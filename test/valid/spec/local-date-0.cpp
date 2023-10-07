TEST(valid_spec_tests, test_local_date_0)
{
    const string toml("ld1 = 1979-05-27\n", 17);

    const Table expected = {{ std::string("ld1", 3), Value(LocalDate(date::year(1979) / date::month(5) / date::day(27))) }};

    assert_parsed(toml, expected);
}
