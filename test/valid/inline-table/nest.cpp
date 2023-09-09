TEST(valid_inline_table_tests, test_nest)
{
    const string toml{"tbl_tbl_empty = { tbl_0 = {} }\ntbl_tbl_val   = { tbl_1 = { one = 1 } }\ntbl_arr_tbl   = { arr_tbl = [ { one = 1 } ] }\narr_tbl_tbl   = [ { tbl = { one = 1 } } ]\n\n# Array-of-array-of-table is interesting because it can only\n# be represented in inline form.\narr_arr_tbl_empty = [ [ {} ] ]\narr_arr_tbl_val = [ [ { one = 1 } ] ]\narr_arr_tbls  = [ [ { one = 1 }, { two = 2 } ] ]\n", 372};

    const Table expected{{ "arr_arr_tbl_empty", Value(Array({Value(Array({Value(Table({}))}))})) },{ "arr_arr_tbl_val", Value(Array({Value(Array({Value(Table({{ "one", Value(INT64_C(1)) }}))}))})) },{ "arr_arr_tbls", Value(Array({Value(Array({Value(Table({{ "one", Value(INT64_C(1)) }})),Value(Table({{ "two", Value(INT64_C(2)) }}))}))})) },{ "arr_tbl_tbl", Value(Array({Value(Table({{ "tbl", Value(Table({{ "one", Value(INT64_C(1)) }})) }}))})) },{ "tbl_arr_tbl", Value(Table({{ "arr_tbl", Value(Array({Value(Table({{ "one", Value(INT64_C(1)) }}))})) }})) },{ "tbl_tbl_empty", Value(Table({{ "tbl_0", Value(Table({})) }})) },{ "tbl_tbl_val", Value(Table({{ "tbl_1", Value(Table({{ "one", Value(INT64_C(1)) }})) }})) }};

    assert_parsed(toml, expected);
}
