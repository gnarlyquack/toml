TEST(invalid_table_tests, test_with_pound)
{
    const string toml{"[key#group]\nanswer = 42\n", 24};

    const vector<Error> expected{{ 1, 2, "Invalid key: key#group" },};

    assert_errors(toml, expected);
}
