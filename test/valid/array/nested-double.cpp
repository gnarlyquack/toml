#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_nested_double)
{
    const string toml{"nest = [\n\t[\n\t\t[\"a\"],\n\t\t[1, 2, [3]]\n\t]\n]\n", 40};

    const Table expected{{ "nest", new ArrayValue({new ArrayValue({new ArrayValue({new StringValue("a"),}),new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new ArrayValue({new IntegerValue(3ULL),}),}),}),}) },};

    assert_parsed(toml, expected);
}
