TEST(invalid_key_tests, test_newline)
{
    const string toml{"barekey\n   = 123\n", 17};

    const vector<Error> expected{{ 1, 8, "Missing '=' between key and value." },{ 1, 8, "Missing value." },{ 2, 4, "Missing key." },};

    assert_errors(toml, expected);
}
