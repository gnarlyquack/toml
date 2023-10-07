TEST(valid_array_tests, test_mixed_string_table)
{
    const string toml("contributors = [\n  \"Foo Bar <foo@example.com>\",\n  { name = \"Baz Qux\", email = \"bazqux@example.com\", url = \"https://example.com/bazqux\" }\n]\n\n# Start with a table as the first element. This tests a case that some libraries\n# might have where they will check if the first entry is a table/map/hash/assoc\n# array and then encode it as a table array. This was a reasonable thing to do\n# before TOML 1.0 since arrays could only contain one type, but now it's no\n# longer.\nmixed = [{k=\"a\"}, \"b\", 1]\n", 492);

    const Table expected = {{ std::string("contributors", 12), Value(Array({Value(std::string("Foo Bar <foo@example.com>", 25)),Value(Table({{ std::string("email", 5), Value(std::string("bazqux@example.com", 18)) },{ std::string("name", 4), Value(std::string("Baz Qux", 7)) },{ std::string("url", 3), Value(std::string("https://example.com/bazqux", 26)) }}))})) },{ std::string("mixed", 5), Value(Array({Value(Table({{ std::string("k", 1), Value(std::string("a", 1)) }})),Value(std::string("b", 1)),Value(INT64_C(1))})) }};

    assert_parsed(toml, expected);
}
