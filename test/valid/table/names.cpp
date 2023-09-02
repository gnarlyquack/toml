#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_names)
{
    const string toml{"[a.b.c]\n[a.\"b.c\"]\n[a.'d.e']\n[a.' x ']\n[ d.e.f ]\n[ g . h . i ]\n[ j . \"\312\236\" . 'l' ]\n\n[x.1.2]\n", 90};

    const Table expected{{ "a", new TableValue({{ " x ", new TableValue({}) },{ "b", new TableValue({{ "c", new TableValue({}) },}) },{ "b.c", new TableValue({}) },{ "d.e", new TableValue({}) },}) },{ "d", new TableValue({{ "e", new TableValue({{ "f", new TableValue({}) },}) },}) },{ "g", new TableValue({{ "h", new TableValue({{ "i", new TableValue({}) },}) },}) },{ "j", new TableValue({{ "ʞ", new TableValue({{ "l", new TableValue({}) },}) },}) },{ "x", new TableValue({{ "1", new TableValue({{ "2", new TableValue({}) },}) },}) },};

    assert_parsed(toml, expected);
}
