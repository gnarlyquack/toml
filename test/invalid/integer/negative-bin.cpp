#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_negative_bin)
{
    const string toml{"negative-bin = -0b11010110\n", 27};

    const vector<Error> expected{{ 1, 16, "A leading '+' or '-' is not allowed in binary integer: 0b11010110." },};

    assert_errors(toml, expected);
}
