#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_bad_uni_esc_1)
{
    const string toml{"str = \"val\\ue\"\n", 15};

    const vector<Error> expected{
        { 1, 11, "Invalid or incomplete Unicode escape sequence: expected 4 hexadecimal characters but parsed 1." },
    };

    assert_errors(toml, expected);
}
