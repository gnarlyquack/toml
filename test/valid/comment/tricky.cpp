TEST(valid_comment_tests, test_tricky)
{
    const string toml{"[section]#attached comment\n#[notsection]\none = \"11\"#cmt\ntwo = \"22#\"\nthree = '#'\n\nfour = \"\"\"# no comment\n# nor this\n#also not comment\"\"\"#is_comment\n\nfive = 5.5#66\nsix = 6#7\n8 = \"eight\"\n#nine = 99\nten = 10e2#1\neleven = 1.11e1#23\n\n[\"hash#tag\"]\n\"#!\" = \"hash bang\"\narr3 = [ \"#\", '#', \"\"\"###\"\"\" ]\narr4 = [ 1,# 9, 9,\n2#,9\n,#9\n3#]\n,4]\narr5 = [[[[#[\"#\"],\n[\"#\"]]]]#]\n]\ntbl1 = { \"#\" = '}#'}#}}\n\n\n", 385};

    const Table expected{{ "hash#tag", Value(Table({{ "#!", Value(std::string("hash bang")) },{ "arr3", Value(Array({Value(std::string("#")),Value(std::string("#")),Value(std::string("###"))})) },{ "arr4", Value(Array({Value(INT64_C(1)),Value(INT64_C(2)),Value(INT64_C(3)),Value(INT64_C(4))})) },{ "arr5", Value(Array({Value(Array({Value(Array({Value(Array({Value(Array({Value(std::string("#"))}))}))}))}))})) },{ "tbl1", Value(Table({{ "#", Value(std::string("}#")) }})) }})) },{ "section", Value(Table({{ "8", Value(std::string("eight")) },{ "eleven", Value(11.1) },{ "five", Value(5.5) },{ "four", Value(std::string("# no comment\n# nor this\n#also not comment")) },{ "one", Value(std::string("11")) },{ "six", Value(INT64_C(6)) },{ "ten", Value(1000.0) },{ "three", Value(std::string("#")) },{ "two", Value(std::string("22#")) }})) }};

    assert_parsed(toml, expected);
}
