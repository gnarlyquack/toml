TEST(valid_inline_table_tests, test_empty)
{
    const string toml("empty1 = {}\nempty2 = { }\nempty_in_array = [ { not_empty = 1 }, {} ]\nempty_in_array2 = [{},{not_empty=1}]\nmany_empty = [{},{},{}]\nnested_empty = {\"empty\"={}}\n", 157);

    const Table expected = {{ std::string("empty1", 6), Value(Table({})) },{ std::string("empty2", 6), Value(Table({})) },{ std::string("empty_in_array", 14), Value(Array({Value(Table({{ std::string("not_empty", 9), Value(INT64_C(1)) }})),Value(Table({}))})) },{ std::string("empty_in_array2", 15), Value(Array({Value(Table({})),Value(Table({{ std::string("not_empty", 9), Value(INT64_C(1)) }}))})) },{ std::string("many_empty", 10), Value(Array({Value(Table({})),Value(Table({})),Value(Table({}))})) },{ std::string("nested_empty", 12), Value(Table({{ std::string("empty", 5), Value(Table({})) }})) }};

    assert_parsed(toml, expected);
}
