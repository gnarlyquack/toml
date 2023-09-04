TEST(invalid_table_tests, test_append_with_dotted_keys_2)
{
    const string toml{"# This is the same issue as in injection-1.toml, except that nests one level\n# deeper. See that file for a more complete description.\n\n[a.b.c.d]\n  z = 9\n\n[a]\n  b.c.d.k.t = \"Using dotted keys to add to [a.b.c.d] after explicitly defining it above is not allowed\"\n", 262};

    const vector<Error> expected{{ 8, 7, "Key 'd' has already been defined on line 4, character 8." },};

    assert_errors(toml, expected);
}
