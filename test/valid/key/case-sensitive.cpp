TEST(valid_key_tests, test_case_sensitive)
{
    const string toml("sectioN = \"NN\"\n\n[section]\nname = \"lower\"\nNAME = \"upper\"\nName = \"capitalized\"\n\n[Section]\nname = \"different section!!\"\n\"\316\274\" = \"greek small letter mu\"\n\"\316\234\" = \"greek capital letter MU\"\nM = \"latin letter M\"\n\n", 203);

    const Table expected = {{ "Section", Value(Table({{ "M", Value(std::string("latin letter M")) },{ "name", Value(std::string("different section!!")) },{ "Μ", Value(std::string("greek capital letter MU")) },{ "μ", Value(std::string("greek small letter mu")) }})) },{ "sectioN", Value(std::string("NN")) },{ "section", Value(Table({{ "NAME", Value(std::string("upper")) },{ "Name", Value(std::string("capitalized")) },{ "name", Value(std::string("lower")) }})) }};

    assert_parsed(toml, expected);
}
