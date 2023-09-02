#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_encoding_tests, test_utf16_bom)
{
    const string toml{"\376\377\000#\000 \000U\000T\000F\000-\0001\0006\000 \000w\000i\000t\000h\000 \000B\000O\000M\000\n", 38};

    const vector<Error> expected{{ 0, 0, "" }};

    assert_errors(toml, expected);
}
