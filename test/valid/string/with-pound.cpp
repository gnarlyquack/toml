TEST(valid_string_tests, test_with_pound)
{
    const string toml("pound = \"We see no # comments here.\"\npoundcomment = \"But there are # some comments here.\" # Did I # mess you up?\n", 113);

    const Table expected = {{ std::string("pound", 5), Value(std::string("We see no # comments here.", 26)) },{ std::string("poundcomment", 12), Value(std::string("But there are # some comments here.", 35)) }};

    assert_parsed(toml, expected);
}
