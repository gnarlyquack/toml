#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_after_array)
{
    const string toml{"[[agencies]] owner = \"S Cjelli\"\n", 32};

    const vector<Error> expected{{ 1, 14, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
