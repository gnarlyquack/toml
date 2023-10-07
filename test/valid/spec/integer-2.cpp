TEST(valid_spec_tests, test_integer_2)
{
    const string toml("# hexadecimal with prefix `0x`\nhex1 = 0xDEADBEEF\nhex2 = 0xdeadbeef\nhex3 = 0xdead_beef\n\n# octal with prefix `0o`\noct1 = 0o01234567\noct2 = 0o755 # useful for Unix file permissions\n\n# binary with prefix `0b`\nbin1 = 0b11010110\n", 223);

    const Table expected = {{ std::string("bin1", 4), Value(INT64_C(214)) },{ std::string("hex1", 4), Value(INT64_C(3735928559)) },{ std::string("hex2", 4), Value(INT64_C(3735928559)) },{ std::string("hex3", 4), Value(INT64_C(3735928559)) },{ std::string("oct1", 4), Value(INT64_C(342391)) },{ std::string("oct2", 4), Value(INT64_C(493)) }};

    assert_parsed(toml, expected);
}
