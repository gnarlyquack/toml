TEST(valid_inline_table_tests, test_nest)
{
    const string toml{"tbl_tbl_empty = { tbl_0 = {} }\ntbl_tbl_val   = { tbl_1 = { one = 1 } }\ntbl_arr_tbl   = { arr_tbl = [ { one = 1 } ] }\narr_tbl_tbl   = [ { tbl = { one = 1 } } ]\n\n# Array-of-array-of-table is interesting because it can only\n# be represented in inline form.\narr_arr_tbl_empty = [ [ {} ] ]\narr_arr_tbl_val = [ [ { one = 1 } ] ]\narr_arr_tbls  = [ [ { one = 1 }, { two = 2 } ] ]\n", 372};

    const Table expected{{ "arr_arr_tbl_empty", Value::of_array({Value::of_array({Value::of_table({})})}) },{ "arr_arr_tbl_val", Value::of_array({Value::of_array({Value::of_table({{ "one", Value::of_integer(1ULL) }})})}) },{ "arr_arr_tbls", Value::of_array({Value::of_array({Value::of_table({{ "one", Value::of_integer(1ULL) }}),Value::of_table({{ "two", Value::of_integer(2ULL) }})})}) },{ "arr_tbl_tbl", Value::of_array({Value::of_table({{ "tbl", Value::of_table({{ "one", Value::of_integer(1ULL) }}) }})}) },{ "tbl_arr_tbl", Value::of_table({{ "arr_tbl", Value::of_array({Value::of_table({{ "one", Value::of_integer(1ULL) }})}) }}) },{ "tbl_tbl_empty", Value::of_table({{ "tbl_0", Value::of_table({}) }}) },{ "tbl_tbl_val", Value::of_table({{ "tbl_1", Value::of_table({{ "one", Value::of_integer(1ULL) }}) }}) }};

    assert_parsed(toml, expected);
}
