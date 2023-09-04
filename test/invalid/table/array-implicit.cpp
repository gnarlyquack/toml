TEST(invalid_table_tests, test_array_implicit)
{
    const string toml{"# This test is a bit tricky. It should fail because the first use of\n# `[[albums.songs]]` without first declaring `albums` implies that `albums`\n# must be a table. The alternative would be quite weird. Namely, it wouldn't\n# comply with the TOML spec: \"Each double-bracketed sub-table will belong to \n# the most *recently* defined table element *above* it.\"\n#\n# This is in contrast to the *valid* test, table-array-implicit where\n# `[[albums.songs]]` works by itself, so long as `[[albums]]` isn't declared\n# later. (Although, `[albums]` could be.)\n[[albums.songs]]\nname = \"Glory Days\"\n\n[[albums]]\nname = \"Born in the USA\"\n", 622};

    const vector<Error> expected{{ 13, 3, "Key 'albums' has already been defined on line 10, character 3." },};

    assert_errors(toml, expected);
}
