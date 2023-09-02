#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_table_tests, test_array_nest)
{
    const string toml{"[[albums]]\nname = \"Born to Run\"\n\n  [[albums.songs]]\n  name = \"Jungleland\"\n\n  [[albums.songs]]\n  name = \"Meeting Across the River\"\n\n[[albums]]\nname = \"Born in the USA\"\n  \n  [[albums.songs]]\n  name = \"Glory Days\"\n\n  [[albums.songs]]\n  name = \"Dancing in the Dark\"\n", 262};

    const Table expected{{ "albums", new ArrayValue({new TableValue({{ "name", new StringValue("Born to Run") },{ "songs", new ArrayValue({new TableValue({{ "name", new StringValue("Jungleland") },}),new TableValue({{ "name", new StringValue("Meeting Across the River") },}),}) },}),new TableValue({{ "name", new StringValue("Born in the USA") },{ "songs", new ArrayValue({new TableValue({{ "name", new StringValue("Glory Days") },}),new TableValue({{ "name", new StringValue("Dancing in the Dark") },}),}) },}),}) },};

    assert_parsed(toml, expected);
}
