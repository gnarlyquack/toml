#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_unicode_escape)
{
    const string toml{"answer4 = \"\\u03B4\"\nanswer8 = \"\\U000003B4\"\n", 42};

    const Table expected{{ "answer4", new StringValue("δ") },{ "answer8", new StringValue("δ") },};

    assert_parsed(toml, expected);
}
