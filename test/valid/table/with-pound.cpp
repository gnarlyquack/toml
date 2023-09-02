#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_with_pound)
{
    const string toml{"[\"key#group\"]\nanswer = 42\n", 26};

    const Table expected{{ "key#group", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },};

    assert_parsed(toml, expected);
}
