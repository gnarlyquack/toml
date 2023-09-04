TEST(invalid_encoding_tests, test_bom_not_at_start_1)
{
    const string toml{"bom-not-at-start \377\375\n", 20};

    const vector<Error> expected{
        { 1, 18, "Missing '=' between key and value." },
        { 1, 18, "Invalid UTF-8 byte: 0xFF" },
        { 1, 19, "Invalid UTF-8 byte: 0xFD" },
    };

    assert_errors(toml, expected);
}
