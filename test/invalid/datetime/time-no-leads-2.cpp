TEST(invalid_datetime_tests, test_time_no_leads_2)
{
    const string toml("# Leading 0 is always required.\nd = 01:32:0\n", 44);

    const vector<Error> expected = {
        { 42, 2, 11, "Second must be two digits." },
    };

    assert_errors(toml, expected);
}
