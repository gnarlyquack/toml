#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_integer_tests, test_literals)
{
    const string toml{"bin1 = 0b11010110\nbin2 = 0b1_0_1\n\noct1 = 0o01234567\noct2 = 0o755\noct3 = 0o7_6_5\n\nhex1 = 0xDEADBEEF\nhex2 = 0xdeadbeef\nhex3 = 0xdead_beef\nhex4 = 0x00987\n", 151};

    const Table expected{{ "bin1", new IntegerValue(214ULL) },{ "bin2", new IntegerValue(5ULL) },{ "hex1", new IntegerValue(3735928559ULL) },{ "hex2", new IntegerValue(3735928559ULL) },{ "hex3", new IntegerValue(3735928559ULL) },{ "hex4", new IntegerValue(2439ULL) },{ "oct1", new IntegerValue(342391ULL) },{ "oct2", new IntegerValue(493ULL) },{ "oct3", new IntegerValue(501ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_integer_tests, test_underscore)
{
    const string toml{"kilo = 1_000\nx = 1_1_1_1\n", 25};

    const Table expected{{ "kilo", new IntegerValue(1000ULL) },{ "x", new IntegerValue(1111ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_integer_tests, test_zero)
{
    const string toml{"d1 = 0\nd2 = +0\nd3 = -0\n\nh1 = 0x0\nh2 = 0x00\nh3 = 0x00000\n\no1 = 0o0\na2 = 0o00\na3 = 0o00000\n\nb1 = 0b0\nb2 = 0b00\nb3 = 0b00000\n", 122};

    const Table expected{{ "a2", new IntegerValue(0ULL) },{ "a3", new IntegerValue(0ULL) },{ "b1", new IntegerValue(0ULL) },{ "b2", new IntegerValue(0ULL) },{ "b3", new IntegerValue(0ULL) },{ "d1", new IntegerValue(0ULL) },{ "d2", new IntegerValue(0ULL) },{ "d3", new IntegerValue(0ULL) },{ "h1", new IntegerValue(0ULL) },{ "h2", new IntegerValue(0ULL) },{ "h3", new IntegerValue(0ULL) },{ "o1", new IntegerValue(0ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_integer_tests, test_integer)
{
    const string toml{"answer = 42\nposanswer = +42\nneganswer = -42\nzero = 0\n", 53};

    const Table expected{{ "answer", new IntegerValue(42ULL) },{ "neganswer", new IntegerValue(-42ULL) },{ "posanswer", new IntegerValue(42ULL) },{ "zero", new IntegerValue(0ULL) },};

    assert_parsed(toml, expected);
}


TEST(valid_integer_tests, test_long)
{
    const string toml{"int64-max = 9223372036854775807\nint64-max-neg = -9223372036854775808\n", 69};

    const Table expected{{ "int64-max", new IntegerValue(9223372036854775807ULL) },{ "int64-max-neg", new IntegerValue(-9223372036854775808ULL) },};

    assert_parsed(toml, expected);
}
