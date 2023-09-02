#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_positive_hex)
{
    const string toml{"positive-hex = +0xff\n", 21};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in hexadecimal integer: 0xff." },};

    assert_errors(toml, expected);
}
