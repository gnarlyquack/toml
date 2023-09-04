#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_encoding_tests, test_bad_utf8_in_string_literal)
{
    const string toml{"# The following line contains an invalid UTF-8 sequence.\nbad = '\303'\n", 67};

    const vector<Error> expected{
        { 2, 8, "Invalid UTF-8 continuation byte: 0x27. Expected a byte in the range of 0x80-0xBF." },
    };

    assert_errors(toml, expected);
}
