#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_with_single_quotes)
{
    const string toml{"['a']\n[a.'b']\n[a.'b'.c]\nanswer = 42 \n", 37};

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },}) },};

    assert_parsed(toml, expected);
}
