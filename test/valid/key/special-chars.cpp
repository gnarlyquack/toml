#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_key_tests, test_special_chars)
{
    const string toml{"\"=~!@$^&*()_+-`1234567890[]|/?><.,;:'=\" = 1\n", 44};

    const Table expected{{ "=~!@$^&*()_+-`1234567890[]|/?><.,;:'=", new IntegerValue(1ULL) },};

    assert_parsed(toml, expected);
}
