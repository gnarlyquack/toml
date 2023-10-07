TEST(valid_comment_tests, test_nonascii)
{
    const string toml("# ~ \302\200 \303\277 \355\237\277 \356\200\200 \357\277\277 \360\220\200\200 \364\217\277\277\n", 32);

    const Table expected = {};

    assert_parsed(toml, expected);
}
