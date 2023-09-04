TEST(valid_string_tests, test_with_pound)
{
    const string toml{"pound = \"We see no # comments here.\"\npoundcomment = \"But there are # some comments here.\" # Did I # mess you up?\n", 113};

    const Table expected{{ "pound", new StringValue("We see no # comments here.") },{ "poundcomment", new StringValue("But there are # some comments here.") },};

    assert_parsed(toml, expected);
}
