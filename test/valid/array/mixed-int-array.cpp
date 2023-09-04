TEST(valid_array_tests, test_mixed_int_array)
{
    const string toml{"arrays-and-ints =  [1, [\"Arrays are not integers.\"]]\n", 53};

    const Table expected{{ "arrays-and-ints", new ArrayValue({new IntegerValue(1ULL),new ArrayValue({new StringValue("Arrays are not integers."),}),}) },};

    assert_parsed(toml, expected);
}
