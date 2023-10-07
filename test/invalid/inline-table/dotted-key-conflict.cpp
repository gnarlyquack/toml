TEST(invalid_inline_table_tests, test_dotted_key_conflict)
{
    const string toml("tbl = { a.b = \"a_b\", a.b.c = \"a_b_c\" }\n", 39);

    const vector<Error> expected = {
        { 23, 1, 24, "Key 'b' has already been defined on line 1, character 11." },
    };

    assert_errors(toml, expected);
}
