TEST(valid_spec_tests, test_array_of_tables_2)
{
    const string toml("points = [ { x = 1, y = 2, z = 3 },\n           { x = 7, y = 8, z = 9 },\n           { x = 2, y = 4, z = 8 } ]\n", 109);

    const Table expected = {{ std::string("points", 6), Value(Array({Value(Table({{ std::string("x", 1), Value(INT64_C(1)) },{ std::string("y", 1), Value(INT64_C(2)) },{ std::string("z", 1), Value(INT64_C(3)) }})),Value(Table({{ std::string("x", 1), Value(INT64_C(7)) },{ std::string("y", 1), Value(INT64_C(8)) },{ std::string("z", 1), Value(INT64_C(9)) }})),Value(Table({{ std::string("x", 1), Value(INT64_C(2)) },{ std::string("y", 1), Value(INT64_C(4)) },{ std::string("z", 1), Value(INT64_C(8)) }}))})) }};

    assert_parsed(toml, expected);
}
