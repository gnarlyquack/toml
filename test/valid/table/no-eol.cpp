TEST(valid_table_tests, test_no_eol)
{
    const string toml{"[table]\n", 8};

    const Table expected{{ "table", new TableValue({}) },};

    assert_parsed(toml, expected);
}
