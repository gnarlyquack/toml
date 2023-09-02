#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_integer_tests, test_zero)
{
    const string toml{"d1 = 0\nd2 = +0\nd3 = -0\n\nh1 = 0x0\nh2 = 0x00\nh3 = 0x00000\n\no1 = 0o0\na2 = 0o00\na3 = 0o00000\n\nb1 = 0b0\nb2 = 0b00\nb3 = 0b00000\n", 122};

    const Table expected{{ "a2", new IntegerValue(0ULL) },{ "a3", new IntegerValue(0ULL) },{ "b1", new IntegerValue(0ULL) },{ "b2", new IntegerValue(0ULL) },{ "b3", new IntegerValue(0ULL) },{ "d1", new IntegerValue(0ULL) },{ "d2", new IntegerValue(0ULL) },{ "d3", new IntegerValue(0ULL) },{ "h1", new IntegerValue(0ULL) },{ "h2", new IntegerValue(0ULL) },{ "h3", new IntegerValue(0ULL) },{ "o1", new IntegerValue(0ULL) },};

    assert_parsed(toml, expected);
}
