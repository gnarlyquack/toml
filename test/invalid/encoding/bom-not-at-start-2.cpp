#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_encoding_tests, test_bom_not_at_start_2)
{
    const string toml{"bom-not-at-start= \377\375\n", 21};

    const vector<Error> expected{{ 0, 0, "" }};

    assert_errors(toml, expected);
}
