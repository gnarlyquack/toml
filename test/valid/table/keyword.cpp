#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_keyword)
{
    const string toml{"[true]\n\n[false]\n\n[inf]\n\n[nan]\n\n\n", 32};

    const Table expected{{ "true", new TableValue({}) },{ "false", new TableValue({}) },{ "inf", new TableValue({}) },{ "nan", new TableValue({}) },};

    assert_parsed(toml, expected);
}
