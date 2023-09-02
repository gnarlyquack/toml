#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_inline_table_tests, test_nest)
{
    const string toml{"tbl_tbl_empty = { tbl_0 = {} }\ntbl_tbl_val   = { tbl_1 = { one = 1 } }\ntbl_arr_tbl   = { arr_tbl = [ { one = 1 } ] }\narr_tbl_tbl   = [ { tbl = { one = 1 } } ]\n\n# Array-of-array-of-table is interesting because it can only\n# be represented in inline form.\narr_arr_tbl_empty = [ [ {} ] ]\narr_arr_tbl_val = [ [ { one = 1 } ] ]\narr_arr_tbls  = [ [ { one = 1 }, { two = 2 } ] ]\n", 372};

    const Table expected{{ "arr_arr_tbl_empty", new ArrayValue({new ArrayValue({new TableValue({}),}),}) },{ "arr_arr_tbl_val", new ArrayValue({new ArrayValue({new TableValue({{ "one", new IntegerValue(1ULL) },}),}),}) },{ "arr_arr_tbls", new ArrayValue({new ArrayValue({new TableValue({{ "one", new IntegerValue(1ULL) },}),new TableValue({{ "two", new IntegerValue(2ULL) },}),}),}) },{ "arr_tbl_tbl", new ArrayValue({new TableValue({{ "tbl", new TableValue({{ "one", new IntegerValue(1ULL) },}) },}),}) },{ "tbl_arr_tbl", new TableValue({{ "arr_tbl", new ArrayValue({new TableValue({{ "one", new IntegerValue(1ULL) },}),}) },}) },{ "tbl_tbl_empty", new TableValue({{ "tbl_0", new TableValue({}) },}) },{ "tbl_tbl_val", new TableValue({{ "tbl_1", new TableValue({{ "one", new IntegerValue(1ULL) },}) },}) },};

    assert_parsed(toml, expected);
}
