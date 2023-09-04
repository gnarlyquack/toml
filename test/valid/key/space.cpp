TEST(valid_key_tests, test_space)
{
    const string toml{"# Keep whitespace inside quotes keys at all positions.\n\"a b\"   = 1\n\" c d \" = 2\n\n[ \" tbl \" ]\n\"\\ttab\\ttab\\t\" = \"tab\"\n", 115};

    const Table expected{{ " c d ", new IntegerValue(2ULL) },{ " tbl ", new TableValue({{ "	tab	tab	", new StringValue("tab") },}) },{ "a b", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}
