#include "../../test_common.hpp"


using namespace std;
using namespace toml;


TEST(valid_comment_tests, test_noeol)
{
    const string toml{"# single comment without any eol characters", 43};

    const Table expected{};

    assert_parsed(toml, expected);
}
