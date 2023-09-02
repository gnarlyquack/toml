#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_key_tests, test_numeric_dotted)
{
    const string toml{"1.2 = 3\n", 8};

    const Table expected{{ "1", new TableValue({{ "2", new IntegerValue(3ULL) },}) },};

    assert_parsed(toml, expected);
}
