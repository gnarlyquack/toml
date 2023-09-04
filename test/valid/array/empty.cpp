TEST(valid_array_tests, test_empty)
{
    const string toml{"thevoid = [[[[[]]]]]\n", 21};

    const Table expected{{ "thevoid", new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({}),}),}),}),}) },};

    assert_parsed(toml, expected);
}
