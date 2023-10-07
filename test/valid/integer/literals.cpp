TEST(valid_integer_tests, test_literals)
{
    const string toml("bin1 = 0b11010110\nbin2 = 0b1_0_1\n\noct1 = 0o01234567\noct2 = 0o755\noct3 = 0o7_6_5\n\nhex1 = 0xDEADBEEF\nhex2 = 0xdeadbeef\nhex3 = 0xdead_beef\nhex4 = 0x00987\n", 151);

    const Table expected = {{ std::string("bin1", 4), Value(INT64_C(214)) },{ std::string("bin2", 4), Value(INT64_C(5)) },{ std::string("hex1", 4), Value(INT64_C(3735928559)) },{ std::string("hex2", 4), Value(INT64_C(3735928559)) },{ std::string("hex3", 4), Value(INT64_C(3735928559)) },{ std::string("hex4", 4), Value(INT64_C(2439)) },{ std::string("oct1", 4), Value(INT64_C(342391)) },{ std::string("oct2", 4), Value(INT64_C(493)) },{ std::string("oct3", 4), Value(INT64_C(501)) }};

    assert_parsed(toml, expected);
}
