TEST(valid_inline_table_tests, test_nest)
{
    const string toml("tbl_tbl_empty = { tbl_0 = {} }\ntbl_tbl_val   = { tbl_1 = { one = 1 } }\ntbl_arr_tbl   = { arr_tbl = [ { one = 1 } ] }\narr_tbl_tbl   = [ { tbl = { one = 1 } } ]\n\n# Array-of-array-of-table is interesting because it can only\n# be represented in inline form.\narr_arr_tbl_empty = [ [ {} ] ]\narr_arr_tbl_val = [ [ { one = 1 } ] ]\narr_arr_tbls  = [ [ { one = 1 }, { two = 2 } ] ]\n", 372);

    const Table expected = {{ std::string("arr_arr_tbl_empty", 17), Value(Array({Value(Array({Value(Table({}))}))})) },{ std::string("arr_arr_tbl_val", 15), Value(Array({Value(Array({Value(Table({{ std::string("one", 3), Value(INT64_C(1)) }}))}))})) },{ std::string("arr_arr_tbls", 12), Value(Array({Value(Array({Value(Table({{ std::string("one", 3), Value(INT64_C(1)) }})),Value(Table({{ std::string("two", 3), Value(INT64_C(2)) }}))}))})) },{ std::string("arr_tbl_tbl", 11), Value(Array({Value(Table({{ std::string("tbl", 3), Value(Table({{ std::string("one", 3), Value(INT64_C(1)) }})) }}))})) },{ std::string("tbl_arr_tbl", 11), Value(Table({{ std::string("arr_tbl", 7), Value(Array({Value(Table({{ std::string("one", 3), Value(INT64_C(1)) }}))})) }})) },{ std::string("tbl_tbl_empty", 13), Value(Table({{ std::string("tbl_0", 5), Value(Table({})) }})) },{ std::string("tbl_tbl_val", 11), Value(Table({{ std::string("tbl_1", 5), Value(Table({{ std::string("one", 3), Value(INT64_C(1)) }})) }})) }};

    assert_parsed(toml, expected);
}
