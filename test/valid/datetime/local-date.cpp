#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_datetime_tests, test_local_date)
{
    const string toml{"bestdayever = 1987-07-05\n", 25};

    const Table expected{{ "bestdayever", new LocalDateValue(LocalDate(date::year(1987) / date::month(7) / date::day(5))) },};

    assert_parsed(toml, expected);
}
