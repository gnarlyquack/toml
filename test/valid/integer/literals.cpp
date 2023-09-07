TEST(valid_integer_tests, test_literals)
{
    const string toml{"bin1 = 0b11010110\nbin2 = 0b1_0_1\n\noct1 = 0o01234567\noct2 = 0o755\noct3 = 0o7_6_5\n\nhex1 = 0xDEADBEEF\nhex2 = 0xdeadbeef\nhex3 = 0xdead_beef\nhex4 = 0x00987\n", 151};

    const Table expected{{ "bin1", Value::of_integer(214ULL) },{ "bin2", Value::of_integer(5ULL) },{ "hex1", Value::of_integer(3735928559ULL) },{ "hex2", Value::of_integer(3735928559ULL) },{ "hex3", Value::of_integer(3735928559ULL) },{ "hex4", Value::of_integer(2439ULL) },{ "oct1", Value::of_integer(342391ULL) },{ "oct2", Value::of_integer(493ULL) },{ "oct3", Value::of_integer(501ULL) }};

    assert_parsed(toml, expected);
}
