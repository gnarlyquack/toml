#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_array_tests, test_no_close_2)
{
    const string toml{"x = [42 #\n", 10};

    const vector<Error> expected{{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}
