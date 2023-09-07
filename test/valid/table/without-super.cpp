TEST(valid_table_tests, test_without_super)
{
    const string toml{"# [x] you\n# [x.y] don't\n# [x.y.z] need these\n[x.y.z.w] # for this to work\n[x] # defining a super-table afterwards is ok\n", 120};

    const Table expected{{ "x", Value::of_table({{ "y", Value::of_table({{ "z", Value::of_table({{ "w", Value::of_table({}) }}) }}) }}) }};

    assert_parsed(toml, expected);
}
