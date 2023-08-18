#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_tests, test_implicit_and_explicit_before)
{
    const string toml = "[a]\nbetter = 43\n\n[a.b.c]\nanswer = 42\n";

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },{ "better", new IntegerValue(43ULL) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_implicit_groups)
{
    const string toml = "[a.b.c]\nanswer = 42\n";

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_example)
{
    const string toml = "best-day-ever = 1987-07-05T17:45:00Z\n\n[numtheory]\nboring = false\nperfection = [6, 28, 496]\n";

    const Table expected{{ "best-day-ever", new OffsetDateTimeValue(date::sys_days(date::year(1987) / date::month(7) / date::day(5)) + chrono::hours(17) + chrono::minutes(45) + chrono::seconds(0) + chrono::microseconds(0)) },{ "numtheory", new TableValue({{ "boring", new BooleanValue(false) },{ "perfection", new ArrayValue({new IntegerValue(6ULL),new IntegerValue(28ULL),new IntegerValue(496ULL),}) },}) },};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_spec_example_1)
{
    const string toml = "# This is a TOML document. Boom.\n\ntitle = \"TOML Example\"\n\n[owner]\nname = \"Lance Uppercut\"\ndob = 1979-05-27T07:32:00-08:00 # First class dates? Why not?\n\n[database]\nserver = \"192.168.1.1\"\nports = [ 8001, 8001, 8002 ]\nconnection_max = 5000\nenabled = true\n\n[servers]\n\n  # You can indent as you please. Tabs or spaces. TOML don't care.\n  [servers.alpha]\n  ip = \"10.0.0.1\"\n  dc = \"eqdc10\"\n\n  [servers.beta]\n  ip = \"10.0.0.2\"\n  dc = \"eqdc10\"\n\n[clients]\ndata = [ [\"gamma\", \"delta\"], [1, 2] ]\n\n# Line breaks are OK when inside arrays\nhosts = [\n  \"alpha\",\n  \"omega\"\n]\n";

    const Table expected{{ "clients", new TableValue({{ "data", new ArrayValue({new ArrayValue({new StringValue("gamma"),new StringValue("delta"),}),new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),}),}) },{ "hosts", new ArrayValue({new StringValue("alpha"),new StringValue("omega"),}) },}) },{ "database", new TableValue({{ "connection_max", new IntegerValue(5000ULL) },{ "enabled", new BooleanValue(true) },{ "ports", new ArrayValue({new IntegerValue(8001ULL),new IntegerValue(8001ULL),new IntegerValue(8002ULL),}) },{ "server", new StringValue("192.168.1.1") },}) },{ "owner", new TableValue({{ "dob", new OffsetDateTimeValue(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(15) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) },{ "name", new StringValue("Lance Uppercut") },}) },{ "servers", new TableValue({{ "alpha", new TableValue({{ "dc", new StringValue("eqdc10") },{ "ip", new StringValue("10.0.0.1") },}) },{ "beta", new TableValue({{ "dc", new StringValue("eqdc10") },{ "ip", new StringValue("10.0.0.2") },}) },}) },{ "title", new StringValue("TOML Example") },};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_empty_file)
{
    const string toml = "";

    const Table expected{};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_spec_example_1_compact)
{
    const string toml = "#Useless spaces eliminated.\ntitle=\"TOML Example\"\n[owner]\nname=\"Lance Uppercut\"\ndob=1979-05-27T07:32:00-08:00#First class dates\n[database]\nserver=\"192.168.1.1\"\nports=[8001,8001,8002]\nconnection_max=5000\nenabled=true\n[servers]\n[servers.alpha]\nip=\"10.0.0.1\"\ndc=\"eqdc10\"\n[servers.beta]\nip=\"10.0.0.2\"\ndc=\"eqdc10\"\n[clients]\ndata=[[\"gamma\",\"delta\"],[1,2]]\nhosts=[\n\"alpha\",\n\"omega\"\n]\n";

    const Table expected{{ "clients", new TableValue({{ "data", new ArrayValue({new ArrayValue({new StringValue("gamma"),new StringValue("delta"),}),new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),}),}) },{ "hosts", new ArrayValue({new StringValue("alpha"),new StringValue("omega"),}) },}) },{ "database", new TableValue({{ "connection_max", new IntegerValue(5000ULL) },{ "enabled", new BooleanValue(true) },{ "ports", new ArrayValue({new IntegerValue(8001ULL),new IntegerValue(8001ULL),new IntegerValue(8002ULL),}) },{ "server", new StringValue("192.168.1.1") },}) },{ "owner", new TableValue({{ "dob", new OffsetDateTimeValue(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(15) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) },{ "name", new StringValue("Lance Uppercut") },}) },{ "servers", new TableValue({{ "alpha", new TableValue({{ "dc", new StringValue("eqdc10") },{ "ip", new StringValue("10.0.0.1") },}) },{ "beta", new TableValue({{ "dc", new StringValue("eqdc10") },{ "ip", new StringValue("10.0.0.2") },}) },}) },{ "title", new StringValue("TOML Example") },};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_newline_lf)
{
    const string toml = "os = \"unix\"\nnewline = \"lf\"\n";

    const Table expected{{ "newline", new StringValue("lf") },{ "os", new StringValue("unix") },};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_newline_crlf)
{
    const string toml = "os = \"DOS\"\r\nnewline = \"crlf\"\r\n";

    const Table expected{{ "newline", new StringValue("crlf") },{ "os", new StringValue("DOS") },};

    assert_parsed(toml, expected);
}


TEST(valid_tests, test_implicit_and_explicit_after)
{
    const string toml = "[a.b.c]\nanswer = 42\n\n[a]\nbetter = 43\n";

    const Table expected{{ "a", new TableValue({{ "b", new TableValue({{ "c", new TableValue({{ "answer", new IntegerValue(42ULL) },}) },}) },{ "better", new IntegerValue(43ULL) },}) },};

    assert_parsed(toml, expected);
}
