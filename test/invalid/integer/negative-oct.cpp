TEST(invalid_integer_tests, test_negative_oct)
{
    const string toml{"negative-oct = -0o99\n", 21};

    const vector<Error> expected{
        { 1, 16, "'-' is not allowed in an octal integer." },
        { 1, 19, "Invalid octal number: 99" },
    };

    assert_errors(toml, expected);
}
