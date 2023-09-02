#include "../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_tests, test_example)
{
    const string toml{"best-day-ever = 1987-07-05T17:45:00Z\n\n[numtheory]\nboring = false\nperfection = [6, 28, 496]\n", 91};

    const Table expected{{ "best-day-ever", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ "numtheory", new TableValue({{ "boring", new BooleanValue(false) },{ "perfection", new ArrayValue({new IntegerValue(6ULL),new IntegerValue(28ULL),new IntegerValue(496ULL),}) },}) },};

    assert_parsed(toml, expected);
}
