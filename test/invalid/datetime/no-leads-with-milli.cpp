TEST(invalid_datetime_tests, test_no_leads_with_milli)
{
    const string toml{"# Day \"5\" instead of \"05\"; the leading zero is required.\nwith-milli = 1987-07-5T17:45:00.12Z\n", 93};

    const vector<Error> expected{{ 2, 22, "Day must be two digits." },};

    assert_errors(toml, expected);
}
