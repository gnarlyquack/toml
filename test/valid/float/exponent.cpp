#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_float_tests, test_exponent)
{
    const string toml{"lower = 3e2\nupper = 3E2\nneg = 3e-2\npos = 3E+2\nzero = 3e0\npointlower = 3.1e2\npointupper = 3.1E2\nminustenth = -1E-1\n", 114};

    const Table expected{{ "lower", new FloatValue(300.0) },{ "minustenth", new FloatValue(-0.1) },{ "neg", new FloatValue(0.03) },{ "pointlower", new FloatValue(310.0) },{ "pointupper", new FloatValue(310.0) },{ "pos", new FloatValue(300.0) },{ "upper", new FloatValue(300.0) },{ "zero", new FloatValue(3.0) },};

    assert_parsed(toml, expected);
}
