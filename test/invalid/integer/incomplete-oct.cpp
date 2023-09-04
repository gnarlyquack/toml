#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_incomplete_oct)
{
    const string toml{"incomplete-oct = 0o\n", 20};

    const vector<Error> expected{{ 1, 20, "Missing octal number." },};

    assert_errors(toml, expected);
}
