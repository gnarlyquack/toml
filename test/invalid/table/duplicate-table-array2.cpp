TEST(invalid_table_tests, test_duplicate_table_array2)
{
    const string toml{"[[tbl]]\n[tbl]\n", 14};

    const vector<Error> expected{{ 2, 2, "Key 'tbl' has already been defined on line 1, character 3." },};

    assert_errors(toml, expected);
}
