#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_simple)
{
    const string toml{"answer = \"You are not drinking enough whisky.\"\n", 47};

    const Table expected{{ "answer", new StringValue("You are not drinking enough whisky.") },};

    assert_parsed(toml, expected);
}
