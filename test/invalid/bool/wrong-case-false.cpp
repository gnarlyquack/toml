#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_bool_tests, test_wrong_case_false)
{
    const string toml{"b = FALSE\n", 10};

    const vector<Error> expected{{ 1, 5, "Invalid value: FALSE." },};

    assert_errors(toml, expected);
}
