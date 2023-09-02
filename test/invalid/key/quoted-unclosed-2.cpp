#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_key_tests, test_quoted_unclosed_2)
{
    const string toml{"\"key\n", 5};

    const vector<Error> expected{{ 1, 5, "Unterminated string." },{ 1, 5, "Missing '=' between key and value." },{ 1, 5, "Missing value." },};

    assert_errors(toml, expected);
}
