#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_integer_tests, test_integer)
{
    const string toml{"answer = 42\nposanswer = +42\nneganswer = -42\nzero = 0\n", 53};

    const Table expected{{ "answer", new IntegerValue(42ULL) },{ "neganswer", new IntegerValue(-42ULL) },{ "posanswer", new IntegerValue(42ULL) },{ "zero", new IntegerValue(0ULL) },};

    assert_parsed(toml, expected);
}
