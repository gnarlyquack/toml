TEST(valid_array_tests, test_nospaces)
{
    const string toml{"ints = [1,2,3]\n", 15};

    const Table expected{{ "ints", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new IntegerValue(3ULL),}) },};

    assert_parsed(toml, expected);
}
