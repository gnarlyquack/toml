#include "../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_tests, test_implicit_groups)
{
    const string toml{"[a.b.c]\nanswer = 42\n", 20};

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },}) },};

    assert_parsed(toml, expected);
}
