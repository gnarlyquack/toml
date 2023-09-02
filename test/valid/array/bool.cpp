#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_bool)
{
    const string toml{"a = [true, false]\n", 18};

    const Table expected{{ "a", new ArrayValue({new BooleanValue(true),new BooleanValue(false),}) },};

    assert_parsed(toml, expected);
}
