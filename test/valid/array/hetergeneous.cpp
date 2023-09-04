TEST(valid_array_tests, test_hetergeneous)
{
    const string toml{"mixed = [[1, 2], [\"a\", \"b\"], [1.1, 2.1]]\n", 41};

    const Table expected{{ "mixed", new ArrayValue({new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),}),new ArrayValue({new StringValue("a"),new StringValue("b"),}),new ArrayValue({new FloatValue(1.1),new FloatValue(2.1),}),}) },};

    assert_parsed(toml, expected);
}
