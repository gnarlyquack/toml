TEST(valid_table_tests, test_sub)
{
    const string toml("[a]\nkey = 1\n\n# a.extend is a key inside the \"a\" table.\n[a.extend]\nkey = 2\n\n[a.extend.more]\nkey = 3\n", 99);

    const Table expected = {{ std::string("a", 1), Value(Table({{ std::string("extend", 6), Value(Table({{ std::string("key", 3), Value(INT64_C(2)) },{ std::string("more", 4), Value(Table({{ std::string("key", 3), Value(INT64_C(3)) }})) }})) },{ std::string("key", 3), Value(INT64_C(1)) }})) }};

    assert_parsed(toml, expected);
}
