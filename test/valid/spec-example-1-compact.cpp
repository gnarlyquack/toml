TEST(valid_tests, test_spec_example_1_compact)
{
    const string toml("#Useless spaces eliminated.\ntitle=\"TOML Example\"\n[owner]\nname=\"Lance Uppercut\"\ndob=1979-05-27T07:32:00-08:00#First class dates\n[database]\nserver=\"192.168.1.1\"\nports=[8001,8001,8002]\nconnection_max=5000\nenabled=true\n[servers]\n[servers.alpha]\nip=\"10.0.0.1\"\ndc=\"eqdc10\"\n[servers.beta]\nip=\"10.0.0.2\"\ndc=\"eqdc10\"\n[clients]\ndata=[[\"gamma\",\"delta\"],[1,2]]\nhosts=[\n\"alpha\",\n\"omega\"\n]\n", 376);

    const Table expected = {{ "clients", Value(Table({{ "data", Value(Array({Value(Array({Value(std::string("gamma")),Value(std::string("delta"))})),Value(Array({Value(INT64_C(1)),Value(INT64_C(2))}))})) },{ "hosts", Value(Array({Value(std::string("alpha")),Value(std::string("omega"))})) }})) },{ "database", Value(Table({{ "connection_max", Value(INT64_C(5000)) },{ "enabled", Value(true) },{ "ports", Value(Array({Value(INT64_C(8001)),Value(INT64_C(8001)),Value(INT64_C(8002))})) },{ "server", Value(std::string("192.168.1.1")) }})) },{ "owner", Value(Table({{ "dob", Value(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(15) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) },{ "name", Value(std::string("Lance Uppercut")) }})) },{ "servers", Value(Table({{ "alpha", Value(Table({{ "dc", Value(std::string("eqdc10")) },{ "ip", Value(std::string("10.0.0.1")) }})) },{ "beta", Value(Table({{ "dc", Value(std::string("eqdc10")) },{ "ip", Value(std::string("10.0.0.2")) }})) }})) },{ "title", Value(std::string("TOML Example")) }};

    assert_parsed(toml, expected);
}
