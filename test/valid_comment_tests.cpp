#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_comment_tests, test_everywhere)
{
    const string toml{"# Top comment.\n  # Top comment.\n# Top comment.\n\n# [no-extraneous-groups-please]\n\n[group] # Comment\nanswer = 42 # Comment\n# no-extraneous-keys-please = 999\n# Inbetween comment.\nmore = [ # Comment\n  # What about multiple # comments?\n  # Can you handle it?\n  #\n          # Evil.\n# Evil.\n  42, 42, # Comments within arrays are fun.\n  # What about multiple # comments?\n  # Can you handle it?\n  #\n          # Evil.\n# Evil.\n# ] Did I fool you?\n] # Hopefully not.\n\n# Make sure the space between the datetime and \"#\" isn't lexed.\ndt = 1979-05-27T07:32:12-07:00  # c\nd = 1979-05-27 # Comment\n", 582};

    const Table expected{{ "group", new TableValue({{ "answer", new IntegerValue(42ULL) },{ "dt", new OffsetDateTimeValue(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(14) + chrono::minutes(32) + chrono::seconds(12) + chrono::microseconds(0)) },{ "d", new LocalDateValue(LocalDate(date::year(1979) / date::month(5) / date::day(27))) },{ "more", new ArrayValue({new IntegerValue(42ULL),new IntegerValue(42ULL),}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_comment_tests, test_at_eof2)
{
    const string toml{"# This is a full-line comment\nkey = \"value\" # This is a comment at the end of a line\n", 85};

    const Table expected{{ "key", new StringValue("value") },};

    assert_parsed(toml, expected);
}


TEST(valid_comment_tests, test_at_eof)
{
    const string toml{"# This is a full-line comment\nkey = \"value\" # This is a comment at the end of a line\n", 85};

    const Table expected{{ "key", new StringValue("value") },};

    assert_parsed(toml, expected);
}


TEST(valid_comment_tests, test_noeol)
{
    const string toml{"# single comment without any eol characters", 43};

    const Table expected{};

    assert_parsed(toml, expected);
}


TEST(valid_comment_tests, test_tricky)
{
    const string toml{"[section]#attached comment\n#[notsection]\none = \"11\"#cmt\ntwo = \"22#\"\nthree = '#'\n\nfour = \"\"\"# no comment\n# nor this\n#also not comment\"\"\"#is_comment\n\nfive = 5.5#66\nsix = 6#7\n8 = \"eight\"\n#nine = 99\nten = 10e2#1\neleven = 1.11e1#23\n\n[\"hash#tag\"]\n\"#!\" = \"hash bang\"\narr3 = [ \"#\", '#', \"\"\"###\"\"\" ]\narr4 = [ 1,# 9, 9,\n2#,9\n,#9\n3#]\n,4]\narr5 = [[[[#[\"#\"],\n[\"#\"]]]]#]\n]\ntbl1 = { \"#\" = '}#'}#}}\n\n\n", 385};

    const Table expected{{ "hash#tag", new TableValue({{ "#!", new StringValue("hash bang") },{ "arr3", new ArrayValue({new StringValue("#"),new StringValue("#"),new StringValue("###"),}) },{ "arr4", new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),new IntegerValue(3ULL),new IntegerValue(4ULL),}) },{ "arr5", new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({new ArrayValue({new StringValue("#"),}),}),}),}),}) },{ "tbl1", new TableValue({{ "#", new StringValue("}#") },}) },}) },{ "section", new TableValue({{ "8", new StringValue("eight") },{ "eleven", new FloatValue(11.1) },{ "five", new FloatValue(5.5) },{ "four", new StringValue("# no comment\n# nor this\n#also not comment") },{ "one", new StringValue("11") },{ "six", new IntegerValue(6ULL) },{ "ten", new FloatValue(1000.0) },{ "three", new StringValue("#") },{ "two", new StringValue("22#") },}) },};

    assert_parsed(toml, expected);
}
