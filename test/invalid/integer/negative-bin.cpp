#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_negative_bin)
{
    const string toml{"negative-bin = -0b11010110\n", 27};

    const vector<Error> expected{{ 1, 16, "'-' is not allowed in a binary integer." },};

    assert_errors(toml, expected);
}
