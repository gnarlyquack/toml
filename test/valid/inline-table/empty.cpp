TEST(valid_inline_table_tests, test_empty)
{
    const string toml{"empty1 = {}\nempty2 = { }\nempty_in_array = [ { not_empty = 1 }, {} ]\nempty_in_array2 = [{},{not_empty=1}]\nmany_empty = [{},{},{}]\nnested_empty = {\"empty\"={}}\n", 157};

    const Table expected{{ "empty1", Value(Table({})) },{ "empty2", Value(Table({})) },{ "empty_in_array", Value(Array({Value(Table({{ "not_empty", Value(INT64_C(1)) }})),Value(Table({}))})) },{ "empty_in_array2", Value(Array({Value(Table({})),Value(Table({{ "not_empty", Value(INT64_C(1)) }}))})) },{ "many_empty", Value(Array({Value(Table({})),Value(Table({})),Value(Table({}))})) },{ "nested_empty", Value(Table({{ "empty", Value(Table({})) }})) }};

    assert_parsed(toml, expected);
}
