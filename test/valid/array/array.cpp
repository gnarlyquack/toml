#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_array)
{
    const string toml{"ints = [1, 2, 3, ]\nfloats = [1.1, 2.1, 3.1]\nstrings = [\"a\", \"b\", \"c\"]\ndates = [\n  1987-07-05T17:45:00Z,\n  1979-05-27T07:32:00Z,\n  2006-06-01T11:00:00Z,\n]\ncomments = [\n         1,\n         2, #this is ok\n]\n", 205};

    const Table expected{{ "comments", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),}) },{ "dates", new ArrayValue({new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)),new OffsetDateTimeValue(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(7) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)),new OffsetDateTimeValue(date::sys_days(date::year(2006) / date::month(6) / date::day(1)) + chrono::hours(11) + chrono::minutes(0) + chrono::seconds(0) + chrono::microseconds(0)),}) },{ "floats", new ArrayValue({new FloatValue(1.1),new FloatValue(2.1),new FloatValue(3.1),}) },{ "ints", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new IntegerValue(3ULL),}) },{ "strings", new ArrayValue({new StringValue("a"),new StringValue("b"),new StringValue("c"),}) },};

    assert_parsed(toml, expected);
}
