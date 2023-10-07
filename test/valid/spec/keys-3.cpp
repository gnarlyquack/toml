TEST(valid_spec_tests, test_keys_3)
{
    const string toml("name = \"Orange\"\nphysical.color = \"orange\"\nphysical.shape = \"round\"\nsite.\"google.com\" = true\n", 92);

    const Table expected = {{ std::string("name", 4), Value(std::string("Orange", 6)) },{ std::string("physical", 8), Value(Table({{ std::string("color", 5), Value(std::string("orange", 6)) },{ std::string("shape", 5), Value(std::string("round", 5)) }})) },{ std::string("site", 4), Value(Table({{ std::string("google.com", 10), Value(true) }})) }};

    assert_parsed(toml, expected);
}
