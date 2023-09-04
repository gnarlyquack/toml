TEST(invalid_datetime_tests, test_no_leads)
{
    const string toml{"# Month \"7\" instead of \"07\"; the leading zero is required.\nno-leads = 1987-7-05T17:45:00Z\n", 90};

    const vector<Error> expected{{ 2, 17, "Month must be two digits." },};

    assert_errors(toml, expected);
}
