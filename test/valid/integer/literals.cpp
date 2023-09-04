TEST(valid_integer_tests, test_literals)
{
    const string toml{"bin1 = 0b11010110\nbin2 = 0b1_0_1\n\noct1 = 0o01234567\noct2 = 0o755\noct3 = 0o7_6_5\n\nhex1 = 0xDEADBEEF\nhex2 = 0xdeadbeef\nhex3 = 0xdead_beef\nhex4 = 0x00987\n", 151};

    const Table expected{{ "bin1", new IntegerValue(214ULL) },{ "bin2", new IntegerValue(5ULL) },{ "hex1", new IntegerValue(3735928559ULL) },{ "hex2", new IntegerValue(3735928559ULL) },{ "hex3", new IntegerValue(3735928559ULL) },{ "hex4", new IntegerValue(2439ULL) },{ "oct1", new IntegerValue(342391ULL) },{ "oct2", new IntegerValue(493ULL) },{ "oct3", new IntegerValue(501ULL) },};

    assert_parsed(toml, expected);
}
