TEST(valid_array_tests, test_mixed_int_array)
{
    const string toml{"arrays-and-ints =  [1, [\"Arrays are not integers.\"]]\n", 53};

    const Table expected{{ "arrays-and-ints", Value::of_array({Value::of_integer(1ULL),Value::of_array({Value::of_string("Arrays are not integers.")})}) }};

    assert_parsed(toml, expected);
}
