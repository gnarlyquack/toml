#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_quoted_unclosed_1)
{
    const string toml{"\"key = x\n", 9};

    const vector<Error> expected{{ 1, 9, "Unterminated string." },{ 1, 9, "Missing '=' between key and value." },{ 1, 9, "Missing value." },};

    assert_errors(toml, expected);
}
