TEST(valid_integer_tests, test_literals)
{
    const string toml("bin1 = 0b11010110\nbin2 = 0b1_0_1\n\noct1 = 0o01234567\noct2 = 0o755\noct3 = 0o7_6_5\n\nhex1 = 0xDEADBEEF\nhex2 = 0xdeadbeef\nhex3 = 0xdead_beef\nhex4 = 0x00987\n", 151);

    const Table expected = {{ "bin1", Value(INT64_C(214)) },{ "bin2", Value(INT64_C(5)) },{ "hex1", Value(INT64_C(3735928559)) },{ "hex2", Value(INT64_C(3735928559)) },{ "hex3", Value(INT64_C(3735928559)) },{ "hex4", Value(INT64_C(2439)) },{ "oct1", Value(INT64_C(342391)) },{ "oct2", Value(INT64_C(493)) },{ "oct3", Value(INT64_C(501)) }};

    assert_parsed(toml, expected);
}
