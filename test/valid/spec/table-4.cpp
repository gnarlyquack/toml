TEST(valid_spec_tests, test_table_4)
{
    const string toml("# [x] you\n# [x.y] don't\n# [x.y.z] need these\n[x.y.z.w] # for this to work\n\n[x] # defining a super-table afterward is ok\n", 120);

    const Table expected = {{ std::string("x", 1), Value(Table({{ std::string("y", 1), Value(Table({{ std::string("z", 1), Value(Table({{ std::string("w", 1), Value(Table({})) }})) }})) }})) }};

    assert_parsed(toml, expected);
}
