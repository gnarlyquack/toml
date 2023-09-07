TEST(valid_key_tests, test_case_sensitive)
{
    const string toml{"sectioN = \"NN\"\n\n[section]\nname = \"lower\"\nNAME = \"upper\"\nName = \"capitalized\"\n\n[Section]\nname = \"different section!!\"\n\"\316\274\" = \"greek small letter mu\"\n\"\316\234\" = \"greek capital letter MU\"\nM = \"latin letter M\"\n\n", 203};

    const Table expected{{ "Section", Value::of_table({{ "M", Value::of_string("latin letter M") },{ "name", Value::of_string("different section!!") },{ "Μ", Value::of_string("greek capital letter MU") },{ "μ", Value::of_string("greek small letter mu") }}) },{ "sectioN", Value::of_string("NN") },{ "section", Value::of_table({{ "NAME", Value::of_string("upper") },{ "Name", Value::of_string("capitalized") },{ "name", Value::of_string("lower") }}) }};

    assert_parsed(toml, expected);
}
