#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_encoding_tests, test_bom_not_at_start_2)
{
    const string toml{"bom-not-at-start= \377\375\n", 21};

    const vector<Error> expected{
        { 1, 19, "Invalid UTF-8 byte: 0xFF" },
        { 1, 20, "Invalid UTF-8 byte: 0xFD" },
    };

    assert_errors(toml, expected);
}
