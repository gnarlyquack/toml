#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_string_tests, test_unicode_literal)
{
    const string toml{"answer = \"\316\264\"\n", 14};

    const Table expected{{ "answer", new StringValue("δ") },};

    assert_parsed(toml, expected);
}
