#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_empty)
{
    const string toml{"answer = \"\"\n", 12};

    const Table expected{{ "answer", new StringValue("") },};

    assert_parsed(toml, expected);
}
