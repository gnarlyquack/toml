TEST(valid_spec_tests, test_array_of_tables_1)
{
    const string toml("[[fruits]]\nname = \"apple\"\n\n[fruits.physical]  # subtable\ncolor = \"red\"\nshape = \"round\"\n\n[[fruits.varieties]]  # nested array of tables\nname = \"red delicious\"\n\n[[fruits.varieties]]\nname = \"granny smith\"\n\n\n[[fruits]]\nname = \"banana\"\n\n[[fruits.varieties]]\nname = \"plantain\"\n", 271);

    const Table expected = {{ std::string("fruits", 6), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("apple", 5)) },{ std::string("physical", 8), Value(Table({{ std::string("color", 5), Value(std::string("red", 3)) },{ std::string("shape", 5), Value(std::string("round", 5)) }})) },{ std::string("varieties", 9), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("red delicious", 13)) }})),Value(Table({{ std::string("name", 4), Value(std::string("granny smith", 12)) }}))})) }})),Value(Table({{ std::string("name", 4), Value(std::string("banana", 6)) },{ std::string("varieties", 9), Value(Array({Value(Table({{ std::string("name", 4), Value(std::string("plantain", 8)) }}))})) }}))})) }};

    assert_parsed(toml, expected);
}
