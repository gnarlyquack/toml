TEST(valid_key_tests, test_case_sensitive)
{
    const string toml("sectioN = \"NN\"\n\n[section]\nname = \"lower\"\nNAME = \"upper\"\nName = \"capitalized\"\n\n[Section]\nname = \"different section!!\"\n\"\316\274\" = \"greek small letter mu\"\n\"\316\234\" = \"greek capital letter MU\"\nM = \"latin letter M\"\n\n", 203);

    const Table expected = {{ std::string("Section", 7), Value(Table({{ std::string("M", 1), Value(std::string("latin letter M", 14)) },{ std::string("name", 4), Value(std::string("different section!!", 19)) },{ std::string("Μ", 2), Value(std::string("greek capital letter MU", 23)) },{ std::string("μ", 2), Value(std::string("greek small letter mu", 21)) }})) },{ std::string("sectioN", 7), Value(std::string("NN", 2)) },{ std::string("section", 7), Value(Table({{ std::string("NAME", 4), Value(std::string("upper", 5)) },{ std::string("Name", 4), Value(std::string("capitalized", 11)) },{ std::string("name", 4), Value(std::string("lower", 5)) }})) }};

    assert_parsed(toml, expected);
}
