TEST(valid_inline_table_tests, test_empty)
{
    const string toml{"empty1 = {}\nempty2 = { }\nempty_in_array = [ { not_empty = 1 }, {} ]\nempty_in_array2 = [{},{not_empty=1}]\nmany_empty = [{},{},{}]\nnested_empty = {\"empty\"={}}\n", 157};

    const Table expected{{ "empty1", Value::of_table({}) },{ "empty2", Value::of_table({}) },{ "empty_in_array", Value::of_array({Value::of_table({{ "not_empty", Value::of_integer(1ULL) }}),Value::of_table({})}) },{ "empty_in_array2", Value::of_array({Value::of_table({}),Value::of_table({{ "not_empty", Value::of_integer(1ULL) }})}) },{ "many_empty", Value::of_array({Value::of_table({}),Value::of_table({}),Value::of_table({})}) },{ "nested_empty", Value::of_table({{ "empty", Value::of_table({}) }}) }};

    assert_parsed(toml, expected);
}
