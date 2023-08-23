#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_datetime_tests, test_mday_over)
{
    const string toml{"# date-mday       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on\n#                           ; month/year\nd = 2006-01-32T00:00:00-00:00\n", 137};

    const vector<Error> expected{{ 3, 13, "Invalid value for day: 32." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_mday_under)
{
    const string toml{"# date-mday       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on\n#                           ; month/year\nd = 2006-01-00T00:00:00-00:00\n", 137};

    const vector<Error> expected{{ 3, 13, "Invalid value for day: 00." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_time_no_leads_2)
{
    const string toml{"# Leading 0 is always required.\nd = 01:32:0\n", 44};

    const vector<Error> expected{{ 2, 11, "Seconds must be 2 digits." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_time_no_leads)
{
    const string toml{"# Leading 0 is always required.\nd = 1:32:00\n", 44};

    const vector<Error> expected{{ 2, 5, "Hours must be 2 digits." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_no_secs)
{
    const string toml{"# No seconds in time.\nno-secs = 1987-07-05T17:45Z\n", 50};

    const vector<Error> expected{{ 2, 27, "Missing ':' between minute and second." },{ 2, 27, "Missing value for second." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_month_over)
{
    const string toml{"# date-month      = 2DIGIT  ; 01-12\nd = 2006-13-01T00:00:00-00:00\n", 66};

    const vector<Error> expected{{ 2, 10, "Invalid value for month: 13." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_no_leads)
{
    const string toml{"# Month \"7\" instead of \"07\"; the leading zero is required.\nno-leads = 1987-7-05T17:45:00Z\n", 90};

    const vector<Error> expected{{ 2, 17, "Months must be 2 digits." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_minute_over)
{
    const string toml{"# time-minute     = 2DIGIT  ; 00-59\nd = 2006-01-01T00:60:00-00:00\n", 66};

    const vector<Error> expected{{ 2, 19, "Invalid value for minute: 60." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_hour_over)
{
    const string toml{"# time-hour       = 2DIGIT  ; 00-23\nd = 2006-01-01T24:00:00-00:00\n", 66};

    const vector<Error> expected{{ 2, 16, "Invalid value for hour: 24." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_month_under)
{
    const string toml{"# date-month      = 2DIGIT  ; 01-12\nd = 2007-00-01T00:00:00-00:00\n", 66};

    const vector<Error> expected{{ 2, 10, "Invalid value for month: 00." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_trailing_t)
{
    const string toml{"# Date cannot end with trailing T\nd = 2006-01-30T\n", 50};

    const vector<Error> expected{{ 2, 16, "Missing value for hour." },{ 2, 16, "Missing ':' between hour and minute." },{ 2, 16, "Missing value for minute." },{ 2, 16, "Missing ':' between minute and second." },{ 2, 16, "Missing value for second." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_no_leads_with_milli)
{
    const string toml{"# Day \"5\" instead of \"05\"; the leading zero is required.\nwith-milli = 1987-07-5T17:45:00.12Z\n", 93};

    const vector<Error> expected{{ 2, 22, "Days must be 2 digits." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_second_over)
{
    const string toml{"# time-second     = 2DIGIT  ; 00-58, 00-59, 00-60 based on leap second\n#                           ; rules\nd = 2006-01-01T00:00:61-00:00\n", 137};

    const vector<Error> expected{{ 3, 22, "Invalid value for second: 61." },};

    assert_errors(toml, expected);
}


TEST(invalid_datetime_tests, test_no_t)
{
    const string toml{"# No \"t\" or \"T\" between the date and time.\nno-t = 1987-07-0517:45:00Z\n", 70};

    const vector<Error> expected{{ 2, 16, "Invalid value for day: 0517:45:00Z." },};

    assert_errors(toml, expected);
}
