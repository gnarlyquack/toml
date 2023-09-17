TEST(valid_comment_tests, test_everywhere)
{
    const string toml("# Top comment.\n  # Top comment.\n# Top comment.\n\n# [no-extraneous-groups-please]\n\n[group] # Comment\nanswer = 42 # Comment\n# no-extraneous-keys-please = 999\n# Inbetween comment.\nmore = [ # Comment\n  # What about multiple # comments?\n  # Can you handle it?\n  #\n          # Evil.\n# Evil.\n  42, 42, # Comments within arrays are fun.\n  # What about multiple # comments?\n  # Can you handle it?\n  #\n          # Evil.\n# Evil.\n# ] Did I fool you?\n] # Hopefully not.\n\n# Make sure the space between the datetime and \"#\" isn't lexed.\ndt = 1979-05-27T07:32:12-07:00  # c\nd = 1979-05-27 # Comment\n", 582);

    const Table expected = {{ "group", Value(Table({{ "answer", Value(INT64_C(42)) },{ "dt", Value(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(14) + chrono::minutes(32) + chrono::seconds(12) + chrono::microseconds(0)) },{ "d", Value(LocalDate(date::year(1979) / date::month(5) / date::day(27))) },{ "more", Value(Array({Value(INT64_C(42)),Value(INT64_C(42))})) }})) }};

    assert_parsed(toml, expected);
}
