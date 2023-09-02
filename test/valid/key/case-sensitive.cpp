#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_key_tests, test_case_sensitive)
{
    const string toml{"sectioN = \"NN\"\n\n[section]\nname = \"lower\"\nNAME = \"upper\"\nName = \"capitalized\"\n\n[Section]\nname = \"different section!!\"\n\"\316\274\" = \"greek small letter mu\"\n\"\316\234\" = \"greek capital letter MU\"\nM = \"latin letter M\"\n\n", 203};

    const Table expected{{ "Section", new TableValue({{ "M", new StringValue("latin letter M") },{ "name", new StringValue("different section!!") },{ "Μ", new StringValue("greek capital letter MU") },{ "μ", new StringValue("greek small letter mu") },}) },{ "sectioN", new StringValue("NN") },{ "section", new TableValue({{ "NAME", new StringValue("upper") },{ "Name", new StringValue("capitalized") },{ "name", new StringValue("lower") },}) },};

    assert_parsed(toml, expected);
}
