#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_nested)
{
    const string toml{"nest = [[\"a\"], [\"b\"]]\n", 22};

    const Table expected{{ "nest", new ArrayValue({new ArrayValue({new StringValue("a"),}),new ArrayValue({new StringValue("b"),}),}) },};

    assert_parsed(toml, expected);
}
