#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_partial_quoted)
{
    const string toml{"partial\"quoted\" = 5\n", 20};

    const vector<Error> expected{{ 1, 1, "Invalid key: partial\"quoted\"." },};

    assert_errors(toml, expected);
}
