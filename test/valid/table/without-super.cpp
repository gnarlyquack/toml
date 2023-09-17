TEST(valid_table_tests, test_without_super)
{
    const string toml("# [x] you\n# [x.y] don't\n# [x.y.z] need these\n[x.y.z.w] # for this to work\n[x] # defining a super-table afterwards is ok\n", 120);

    const Table expected = {{ "x", Value(Table({{ "y", Value(Table({{ "z", Value(Table({{ "w", Value(Table({})) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
