TEST(invalid_key_tests, test_newline)
{
    const string toml("barekey\n   = 123\n", 17);

    const vector<Error> expected = {
        {  7, 1, 8, "Missing '=' between key and value." },
        {  7, 1, 8, "Missing value." },
        { 11, 2, 4, "Missing key." },
    };

    assert_errors(toml, expected);
}
