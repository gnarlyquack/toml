TEST(invalid_integer_tests, test_positive_bin)
{
    const string toml{"positive-bin = +0b11010110\n", 27};

    const vector<Error> expected{{ 1, 16, "'+' is not allowed in a binary integer." },};

    assert_errors(toml, expected);
}
