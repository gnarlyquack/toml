TEST(valid_key_tests, test_dotted_empty)
{
    const string toml("''.x = \"empty.x\"\nx.\"\" = \"x.empty\"\n[a]\n\"\".'' = \"empty.empty\"\n", 60);

    const Table expected = {{ std::string("", 0), Value(Table({{ std::string("x", 1), Value(std::string("empty.x", 7)) }})) },{ std::string("x", 1), Value(Table({{ std::string("", 0), Value(std::string("x.empty", 7)) }})) },{ std::string("a", 1), Value(Table({{ std::string("", 0), Value(Table({{ std::string("", 0), Value(std::string("empty.empty", 11)) }})) }})) }};

    assert_parsed(toml, expected);
}
