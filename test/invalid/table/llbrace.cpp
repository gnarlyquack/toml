TEST(invalid_table_tests, test_llbrace)
{
    const string toml{"[ [table]]\n", 11};

    const vector<Error> expected{{ 1, 3, "Invalid key: [table" },{ 1, 10, "Expected the end of the line but got: ]" },};

    assert_errors(toml, expected);
}
