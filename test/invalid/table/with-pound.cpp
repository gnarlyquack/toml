TEST(invalid_table_tests, test_with_pound)
{
    const string toml("[key#group]\nanswer = 42\n", 24);

    const vector<Error> expected = {
        { 4, 1, 5, "Missing closing ']' for table header." },
    };

    assert_errors(toml, expected);
}
