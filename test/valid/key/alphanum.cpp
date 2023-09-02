#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_key_tests, test_alphanum)
{
    const string toml{"alpha = \"a\"\n123 = \"num\"\n000111 = \"leading\"\n10e3 = \"false float\"\none1two2 = \"mixed\"\nwith-dash = \"dashed\"\nunder_score = \"___\"\n34-11 = 23\n\n[2018_10]\n001 = 1\n\n[a-a-a]\n_ = false\n", 173};

    const Table expected{{ "000111", new StringValue("leading") },{ "10e3", new StringValue("false float") },{ "123", new StringValue("num") },{ "2018_10", new TableValue({{ "001", new IntegerValue(1ULL) },}) },{ "34-11", new IntegerValue(23ULL) },{ "a-a-a", new TableValue({{ "_", new BooleanValue(false) },}) },{ "alpha", new StringValue("a") },{ "one1two2", new StringValue("mixed") },{ "under_score", new StringValue("___") },{ "with-dash", new StringValue("dashed") },};

    assert_parsed(toml, expected);
}
