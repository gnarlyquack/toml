#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_datetime_tests, test_local_time)
{
    const string toml{"besttimeever = 17:45:00\nmilliseconds = 10:32:00.555\n", 52};

    const Table expected{{ "besttimeever", new LocalTimeValue(LocalTime(chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0))) },{ "milliseconds", new LocalTimeValue(LocalTime(chrono::hours(10) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(555000))) },};

    assert_parsed(toml, expected);
}


TEST(valid_datetime_tests, test_local)
{
    const string toml{"local = 1987-07-05T17:45:00\nmilli = 1977-12-21T10:32:00.555\nspace = 1987-07-05 17:45:00\n", 88};

    const Table expected{{ "local", new LocalDateTimeValue(LocalDate(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ "milli", new LocalDateTimeValue(LocalDate(date::year(1977) / date::month(12) / date::day(21)) + chrono::hours(10) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(555000)) },{ "space", new LocalDateTimeValue(LocalDate(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },};

    assert_parsed(toml, expected);
}


TEST(valid_datetime_tests, test_datetime)
{
    const string toml{"space = 1987-07-05 17:45:00Z\nlower = 1987-07-05t17:45:00z\n", 58};

    const Table expected{{ "lower", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ "space", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },};

    assert_parsed(toml, expected);
}


TEST(valid_datetime_tests, test_local_date)
{
    const string toml{"bestdayever = 1987-07-05\n", 25};

    const Table expected{{ "bestdayever", new LocalDateValue(LocalDate(date::year(1987) / date::month(7) / date::day(5))) },};

    assert_parsed(toml, expected);
}


TEST(valid_datetime_tests, test_timezone)
{
    const string toml{"utc  = 1987-07-05T17:45:56Z\npdt  = 1987-07-05T17:45:56-05:00\nnzst = 1987-07-05T17:45:56+12:00\nnzdt = 1987-07-05T17:45:56+13:00  # DST\n", 134};

    const Table expected{{ "nzdt", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(4) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) },{ "nzst", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(5) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) },{ "pdt", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(22) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) },{ "utc", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(0)) },};

    assert_parsed(toml, expected);
}


TEST(valid_datetime_tests, test_milliseconds)
{
    const string toml{"utc1  = 1987-07-05T17:45:56.1234Z\nutc2  = 1987-07-05T17:45:56.6Z\nwita1 = 1987-07-05T17:45:56.1234+08:00\nwita2 = 1987-07-05T17:45:56.6+08:00\n", 140};

    const Table expected{{ "utc1", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(123400)) },{ "utc2", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(600000)) },{ "wita1", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(9) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(123400)) },{ "wita2", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(9) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(600000)) },};

    assert_parsed(toml, expected);
}
