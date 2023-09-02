#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_integer_tests, test_underscore)
{
    const string toml{"kilo = 1_000\nx = 1_1_1_1\n", 25};

    const Table expected{{ "kilo", new IntegerValue(1000ULL) },{ "x", new IntegerValue(1111ULL) },};

    assert_parsed(toml, expected);
}
