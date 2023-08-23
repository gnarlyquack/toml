#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_float_tests, test_underscore)
{
    const string toml{"before = 3_141.5927\nafter = 3141.592_7\nexponent = 3e1_4\n", 56};

    const Table expected{{ "after", new FloatValue(3141.5927) },{ "before", new FloatValue(3141.5927) },{ "exponent", new FloatValue(3.0e14) },};

    assert_parsed(toml, expected);
}


TEST(valid_float_tests, test_zero)
{
    const string toml{"zero = 0.0\nsigned-pos = +0.0\nsigned-neg = -0.0\nexponent = 0e0\nexponent-two-0 = 0e00\nexponent-signed-pos = +0e0\nexponent-signed-neg = -0e0\n", 138};

    const Table expected{{ "zero", new FloatValue(0) },{ "signed-pos", new FloatValue(0) },{ "signed-neg", new FloatValue(0) },{ "exponent", new FloatValue(0) },{ "exponent-two-0", new FloatValue(0) },{ "exponent-signed-pos", new FloatValue(0) },{ "exponent-signed-neg", new FloatValue(0) },};

    assert_parsed(toml, expected);
}


TEST(valid_float_tests, test_inf_and_nan)
{
    const string toml{"# We don't encode +nan and -nan back with the signs; many languages don't\n# support a sign on NaN (it doesn't really make much sense).\nnan = nan\nnan_neg = -nan\nnan_plus = +nan\ninfinity = inf\ninfinity_neg = -inf\ninfinity_plus = +inf\n", 232};

    const Table expected{{ "infinity", new FloatValue(INF64) },{ "infinity_neg", new FloatValue(-INF64) },{ "infinity_plus", new FloatValue(+INF64) },{ "nan", new FloatValue(NAN64) },{ "nan_neg", new FloatValue(NAN64) },{ "nan_plus", new FloatValue(NAN64) },};

    assert_parsed(toml, expected);
}


TEST(valid_float_tests, test_float)
{
    const string toml{"pi = 3.14\npospi = +3.14\nnegpi = -3.14\nzero-intpart = 0.123\n", 59};

    const Table expected{{ "negpi", new FloatValue(-3.14) },{ "pi", new FloatValue(3.14) },{ "pospi", new FloatValue(3.14) },{ "zero-intpart", new FloatValue(0.123) },};

    assert_parsed(toml, expected);
}


TEST(valid_float_tests, test_exponent)
{
    const string toml{"lower = 3e2\nupper = 3E2\nneg = 3e-2\npos = 3E+2\nzero = 3e0\npointlower = 3.1e2\npointupper = 3.1E2\nminustenth = -1E-1\n", 114};

    const Table expected{{ "lower", new FloatValue(300.0) },{ "minustenth", new FloatValue(-0.1) },{ "neg", new FloatValue(0.03) },{ "pointlower", new FloatValue(310.0) },{ "pointupper", new FloatValue(310.0) },{ "pos", new FloatValue(300.0) },{ "upper", new FloatValue(300.0) },{ "zero", new FloatValue(3.0) },};

    assert_parsed(toml, expected);
}


TEST(valid_float_tests, test_long)
{
    const string toml{"longpi = 3.141592653589793\nneglongpi = -3.141592653589793\n", 58};

    const Table expected{{ "longpi", new FloatValue(3.141592653589793) },{ "neglongpi", new FloatValue(-3.141592653589793) },};

    assert_parsed(toml, expected);
}
