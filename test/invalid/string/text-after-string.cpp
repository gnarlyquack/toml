#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_string_tests, test_text_after_string)
{
    const string toml{"string = \"Is there life after strings?\" No.\n", 44};

    const vector<Error> expected{{ 1, 41, "Expected a new line after expression." },};

    assert_errors(toml, expected);
}
