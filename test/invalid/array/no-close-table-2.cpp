#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_array_tests, test_no_close_table_2)
{
    const string toml{"x = [{ key = 42 #\n", 18};

    const vector<Error> expected{{ 1, 17, "Missing ',' between inline table values." },{ 1, 17, "Invalid key: #" },{ 1, 18, "Missing '=' between key and value." },{ 1, 18, "Missing value." },{ 1, 18, "Unterminated inline table." },{ 2, 1, "Unterminated array." },};

    assert_errors(toml, expected);
}
