TEST(valid_array_tests, test_mixed_string_table)
{
    const string toml{"contributors = [\n  \"Foo Bar <foo@example.com>\",\n  { name = \"Baz Qux\", email = \"bazqux@example.com\", url = \"https://example.com/bazqux\" }\n]\n\n# Start with a table as the first element. This tests a case that some libraries\n# might have where they will check if the first entry is a table/map/hash/assoc\n# array and then encode it as a table array. This was a reasonable thing to do\n# before TOML 1.0 since arrays could only contain one type, but now it's no\n# longer.\nmixed = [{k=\"a\"}, \"b\", 1]\n", 492};

    const Table expected{{ "contributors", Value::of_array({Value::of_string("Foo Bar <foo@example.com>"),Value::of_table({{ "email", Value::of_string("bazqux@example.com") },{ "name", Value::of_string("Baz Qux") },{ "url", Value::of_string("https://example.com/bazqux") }})}) },{ "mixed", Value::of_array({Value::of_table({{ "k", Value::of_string("a") }}),Value::of_string("b"),Value::of_integer(1ULL)}) }};

    assert_parsed(toml, expected);
}
