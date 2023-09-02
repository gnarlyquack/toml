#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_control_tests, test_bare_cr)
{
    const string toml{"# The following line contains a single carriage return control character\r\n\r", 75};

    const vector<Error> expected{{ 2, 1, "Invalid key: \r." },{ 2, 2, "Missing '=' between key and value." },{ 2, 2, "Missing value." },};

    assert_errors(toml, expected);
}
