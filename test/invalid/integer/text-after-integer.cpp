#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_integer_tests, test_text_after_integer)
{
    const string toml{"answer = 42 the ultimate answer?\n", 33};

    const vector<Error> expected{{ 1, 13, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
