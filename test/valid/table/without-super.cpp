TEST(valid_table_tests, test_without_super)
{
    const string toml("# [x] you\n# [x.y] don't\n# [x.y.z] need these\n[x.y.z.w] # for this to work\n[x] # defining a super-table afterwards is ok\n", 120);

    const Table expected = {{ std::string("x", 1), Value(Table({{ std::string("y", 1), Value(Table({{ std::string("z", 1), Value(Table({{ std::string("w", 1), Value(Table({})) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
