TEST(valid_comment_tests, test_tricky)
{
    const string toml{"[section]#attached comment\n#[notsection]\none = \"11\"#cmt\ntwo = \"22#\"\nthree = '#'\n\nfour = \"\"\"# no comment\n# nor this\n#also not comment\"\"\"#is_comment\n\nfive = 5.5#66\nsix = 6#7\n8 = \"eight\"\n#nine = 99\nten = 10e2#1\neleven = 1.11e1#23\n\n[\"hash#tag\"]\n\"#!\" = \"hash bang\"\narr3 = [ \"#\", '#', \"\"\"###\"\"\" ]\narr4 = [ 1,# 9, 9,\n2#,9\n,#9\n3#]\n,4]\narr5 = [[[[#[\"#\"],\n[\"#\"]]]]#]\n]\ntbl1 = { \"#\" = '}#'}#}}\n\n\n", 385};

    const Table expected{{ "hash#tag", new TableValue({{ "#!", new StringValue("hash bang") },{ "arr3", new ArrayValue({new StringValue("#"),new StringValue("#"),new StringValue("###"),}) },{ "arr4", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new IntegerValue(3ULL),new IntegerValue(4ULL),}) },{ "arr5", new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({new StringValue("#"),}),}),}),}),}) },{ "tbl1", new TableValue({{ "#", new StringValue("}#") },}) },}) },{ "section", new TableValue({{ "8", new StringValue("eight") },{ "eleven", new FloatValue(11.1) },{ "five", new FloatValue(5.5) },{ "four", new StringValue("# no comment\n# nor this\n#also not comment") },{ "one", new StringValue("11") },{ "six", new IntegerValue(6ULL) },{ "ten", new FloatValue(1000.0) },{ "three", new StringValue("#") },{ "two", new StringValue("22#") },}) },};

    assert_parsed(toml, expected);
}
