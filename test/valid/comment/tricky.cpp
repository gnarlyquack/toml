TEST(valid_comment_tests, test_tricky)
{
    const string toml{"[section]#attached comment\n#[notsection]\none = \"11\"#cmt\ntwo = \"22#\"\nthree = '#'\n\nfour = \"\"\"# no comment\n# nor this\n#also not comment\"\"\"#is_comment\n\nfive = 5.5#66\nsix = 6#7\n8 = \"eight\"\n#nine = 99\nten = 10e2#1\neleven = 1.11e1#23\n\n[\"hash#tag\"]\n\"#!\" = \"hash bang\"\narr3 = [ \"#\", '#', \"\"\"###\"\"\" ]\narr4 = [ 1,# 9, 9,\n2#,9\n,#9\n3#]\n,4]\narr5 = [[[[#[\"#\"],\n[\"#\"]]]]#]\n]\ntbl1 = { \"#\" = '}#'}#}}\n\n\n", 385};

    const Table expected{{ "hash#tag", Value::of_table({{ "#!", Value::of_string("hash bang") },{ "arr3", Value::of_array({Value::of_string("#"),Value::of_string("#"),Value::of_string("###")}) },{ "arr4", Value::of_array({Value::of_integer(1ULL),Value::of_integer(2ULL),Value::of_integer(3ULL),Value::of_integer(4ULL)}) },{ "arr5", Value::of_array({Value::of_array({Value::of_array({Value::of_array({Value::of_array({Value::of_string("#")})})})})}) },{ "tbl1", Value::of_table({{ "#", Value::of_string("}#") }}) }}) },{ "section", Value::of_table({{ "8", Value::of_string("eight") },{ "eleven", Value::of_float(11.1) },{ "five", Value::of_float(5.5) },{ "four", Value::of_string("# no comment\n# nor this\n#also not comment") },{ "one", Value::of_string("11") },{ "six", Value::of_integer(6ULL) },{ "ten", Value::of_float(1000.0) },{ "three", Value::of_string("#") },{ "two", Value::of_string("22#") }}) }};

    assert_parsed(toml, expected);
}
