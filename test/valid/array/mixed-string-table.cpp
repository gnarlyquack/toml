TEST(valid_array_tests, test_mixed_string_table)
{
    const string toml("contributors = [\n  \"Foo Bar <foo@example.com>\",\n  { name = \"Baz Qux\", email = \"bazqux@example.com\", url = \"https://example.com/bazqux\" }\n]\n\n# Start with a table as the first element. This tests a case that some libraries\n# might have where they will check if the first entry is a table/map/hash/assoc\n# array and then encode it as a table array. This was a reasonable thing to do\n# before TOML 1.0 since arrays could only contain one type, but now it's no\n# longer.\nmixed = [{k=\"a\"}, \"b\", 1]\n", 492);

    const Table expected = {{ "contributors", Value(Array({Value(std::string("Foo Bar <foo@example.com>")),Value(Table({{ "email", Value(std::string("bazqux@example.com")) },{ "name", Value(std::string("Baz Qux")) },{ "url", Value(std::string("https://example.com/bazqux")) }}))})) },{ "mixed", Value(Array({Value(Table({{ "k", Value(std::string("a")) }})),Value(std::string("b")),Value(INT64_C(1))})) }};

    assert_parsed(toml, expected);
}
