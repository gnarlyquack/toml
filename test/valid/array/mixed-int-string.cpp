#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_array_tests, test_mixed_int_string)
{
    const string toml{"strings-and-ints = [\"hi\", 42]\n", 30};

    const Table expected{{ "strings-and-ints", new ArrayValue({new StringValue("hi"),new IntegerValue(42ULL),}) },};

    assert_parsed(toml, expected);
}
