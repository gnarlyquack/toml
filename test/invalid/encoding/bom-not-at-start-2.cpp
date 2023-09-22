TEST(invalid_encoding_tests, test_bom_not_at_start_2)
{
    const string toml("bom-not-at-start= \377\375\n", 21);

    const vector<Error> expected = {
        { 18, 1, 19, "Invalid UTF-8 byte: 0xFF" },
        { 18, 1, 19, "Invalid value: \377\375" }, // TODO remove this error
        { 19, 1, 20, "Invalid UTF-8 byte: 0xFD" },
    };

    assert_errors(toml, expected);
}
