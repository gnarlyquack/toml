#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_datetime_tests, test_milliseconds)
{
    const string toml{"utc1  = 1987-07-05T17:45:56.1234Z\nutc2  = 1987-07-05T17:45:56.6Z\nwita1 = 1987-07-05T17:45:56.1234+08:00\nwita2 = 1987-07-05T17:45:56.6+08:00\n", 140};

    const Table expected{{ "utc1", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(123400)) },{ "utc2", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(600000)) },{ "wita1", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(9) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(123400)) },{ "wita2", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(9) + chrono::minutes(45) + chrono::seconds(56) + chrono::microseconds(600000)) },};

    assert_parsed(toml, expected);
}
