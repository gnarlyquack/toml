TEST(valid_table_tests, test_empty_name)
{
    const string toml("['']\nx = 1\n\n[\"\".a]\nx = 2\n\n[a.'']\nx = 3\n", 39);

    const Table expected = {{ std::string("", 0), Value(Table({{ std::string("x", 1), Value(INT64_C(1)) },{ std::string("a", 1), Value(Table({{ std::string("x", 1), Value(INT64_C(2)) }})) }})) },{ std::string("a", 1), Value(Table({{ std::string("", 0), Value(Table({{ std::string("x", 1), Value(INT64_C(3)) }})) }})) }};

    assert_parsed(toml, expected);
}
