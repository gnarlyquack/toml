#include "test_common.hpp"


using namespace std;
using namespace toml;


TEST(invalid_bool_tests, test_starting_same_false)
{
    const string toml{"a = falsey\n", 11};

    const vector<Error> expected{{ 1, 5, "Invalid value: falsey." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_just_t)
{
    const string toml{"a = t\n", 6};

    const vector<Error> expected{{ 1, 5, "Invalid value: t." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_almost_true_with_extra)
{
    const string toml{"a = truthy\n", 11};

    const vector<Error> expected{{ 1, 5, "Invalid value: truthy." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_almost_false_with_extra)
{
    const string toml{"a = falsify\n", 12};

    const vector<Error> expected{{ 1, 5, "Invalid value: falsify." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_almost_true)
{
    const string toml{"a = tru\n", 8};

    const vector<Error> expected{{ 1, 5, "Invalid value: tru." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_starting_same_true)
{
    const string toml{"a = truer\n", 10};

    const vector<Error> expected{{ 1, 5, "Invalid value: truer." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_mixed_case)
{
    const string toml{"valid = False\n", 14};

    const vector<Error> expected{{ 1, 9, "Invalid value: False." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_almost_false)
{
    const string toml{"a = fals\n", 9};

    const vector<Error> expected{{ 1, 5, "Invalid value: fals." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_just_f)
{
    const string toml{"a = f\n", 6};

    const vector<Error> expected{{ 1, 5, "Invalid value: f." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_wrong_case_true)
{
    const string toml{"a = TRUE\n", 9};

    const vector<Error> expected{{ 1, 5, "Invalid value: TRUE." },};

    assert_errors(toml, expected);
}


TEST(invalid_bool_tests, test_wrong_case_false)
{
    const string toml{"b = FALSE\n", 10};

    const vector<Error> expected{{ 1, 5, "Invalid value: FALSE." },};

    assert_errors(toml, expected);
}
