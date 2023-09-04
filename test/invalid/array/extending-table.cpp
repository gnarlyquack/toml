#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_array_tests, test_extending_table)
{
    const string toml{"a = [{ b = 1 }]\n\n# Cannot extend tables within static arrays\n# https://github.com/toml-lang/toml/issues/908\n[a.c]\nfoo = 1\n", 122};

    const vector<Error> expected{{ 5, 2, "Key 'a' has already been defined on line 1, character 1." },};

    assert_errors(toml, expected);
}
