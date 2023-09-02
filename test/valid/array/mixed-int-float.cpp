#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_mixed_int_float)
{
    const string toml{"ints-and-floats = [1, 1.1]\n", 27};

    const Table expected{{ "ints-and-floats", new ArrayValue({new IntegerValue(1ULL),new FloatValue(1.1),}) },};

    assert_parsed(toml, expected);
}
