#include "../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_tests, test_implicit_and_explicit_before)
{
    const string toml{"[a]\nbetter = 43\n\n[a.b.c]\nanswer = 42\n", 37};

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },{ "better", new IntegerValue(43ULL) },}) },};

    assert_parsed(toml, expected);
}
