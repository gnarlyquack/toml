TEST(invalid_string_tests, test_text_after_string)
{
    const string toml("string = \"Is there life after strings?\" No.\n", 44);

    const vector<Error> expected = {
        { 40, 1, 41, "Expected the end of the line but got: No." },
    };

    assert_errors(toml, expected);
}
