#include "../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_tests, test_spec_example_1)
{
    const string toml{"# This is a TOML document. Boom.\n\ntitle = \"TOML Example\"\n\n[owner]\nname = \"Lance Uppercut\"\ndob = 1979-05-27T07:32:00-08:00 # First class dates? Why not?\n\n[database]\nserver = \"192.168.1.1\"\nports = [ 8001, 8001, 8002 ]\nconnection_max = 5000\nenabled = true\n\n[servers]\n\n  # You can indent as you please. Tabs or spaces. TOML don't care.\n  [servers.alpha]\n  ip = \"10.0.0.1\"\n  dc = \"eqdc10\"\n\n  [servers.beta]\n  ip = \"10.0.0.2\"\n  dc = \"eqdc10\"\n\n[clients]\ndata = [ [\"gamma\", \"delta\"], [1, 2] ]\n\n# Line breaks are OK when inside arrays\nhosts = [\n  \"alpha\",\n  \"omega\"\n]\n", 559};

    const Table expected{{ "clients", new TableValue({{ "data", new ArrayValue({new ArrayValue({new StringValue("gamma"),new StringValue("delta"),}),new ArrayValue({new IntegerValue(1ULL),new IntegerValue(2ULL),}),}) },{ "hosts", new ArrayValue({new StringValue("alpha"),new StringValue("omega"),}) },}) },{ "database", new TableValue({{ "connection_max", new IntegerValue(5000ULL) },{ "enabled", new BooleanValue(true) },{ "ports", new ArrayValue({new IntegerValue(8001ULL),new IntegerValue(8001ULL),new IntegerValue(8002ULL),}) },{ "server", new StringValue("192.168.1.1") },}) },{ "owner", new TableValue({{ "dob", new OffsetDateTimeValue(date::sys_days(date::year(1979) / date::month(5) / date::day(27)) + chrono::hours(15) + chrono::minutes(32) + chrono::seconds(0) + chrono::microseconds(0)) },{ "name", new StringValue("Lance Uppercut") },}) },{ "servers", new TableValue({{ "alpha", new TableValue({{ "dc", new StringValue("eqdc10") },{ "ip", new StringValue("10.0.0.1") },}) },{ "beta", new TableValue({{ "dc", new StringValue("eqdc10") },{ "ip", new StringValue("10.0.0.2") },}) },}) },{ "title", new StringValue("TOML Example") },};

    assert_parsed(toml, expected);
}
