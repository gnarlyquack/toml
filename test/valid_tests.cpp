#include "test_common.hpp"

using namespace std;
using namespace toml;

#include "valid/integer/literals.cpp"
#include "valid/integer/integer.cpp"
#include "valid/integer/long.cpp"
#include "valid/integer/underscore.cpp"
#include "valid/integer/zero.cpp"
#include "valid/newline-lf.cpp"
#include "valid/example.cpp"
#include "valid/datetime/local.cpp"
#include "valid/datetime/milliseconds.cpp"
#include "valid/datetime/datetime.cpp"
#include "valid/datetime/local-time.cpp"
#include "valid/datetime/local-date.cpp"
#include "valid/datetime/timezone.cpp"
#include "valid/implicit-groups.cpp"
#include "valid/string/empty.cpp"
#include "valid/string/raw-multiline.cpp"
#include "valid/string/unicode-literal.cpp"
#include "valid/string/raw.cpp"
#include "valid/string/multiline-escaped-crlf.cpp"
#include "valid/string/unicode-escape.cpp"
#include "valid/string/double-quote-escape.cpp"
#include "valid/string/with-pound.cpp"
#include "valid/string/escaped-escape.cpp"
#include "valid/string/escape-tricky.cpp"
#include "valid/string/multiline.cpp"
#include "valid/string/nl.cpp"
#include "valid/string/escapes.cpp"
#include "valid/string/simple.cpp"
#include "valid/string/multiline-quotes.cpp"
#include "valid/implicit-and-explicit-after.cpp"
#include "valid/implicit-and-explicit-before.cpp"
#include "valid/spec-example-1.cpp"
#include "valid/inline-table/empty.cpp"
#include "valid/inline-table/end-in-bool.cpp"
#include "valid/inline-table/nest.cpp"
#include "valid/inline-table/bool.cpp"
#include "valid/inline-table/multiline.cpp"
#include "valid/inline-table/key-dotted.cpp"
#include "valid/inline-table/array.cpp"
#include "valid/inline-table/inline-table.cpp"
#include "valid/key/empty.cpp"
#include "valid/key/quoted-dots.cpp"
#include "valid/key/dotted.cpp"
#include "valid/key/numeric.cpp"
#include "valid/key/special-chars.cpp"
#include "valid/key/space.cpp"
#include "valid/key/alphanum.cpp"
#include "valid/key/numeric-dotted.cpp"
#include "valid/key/equals-nospace.cpp"
#include "valid/key/case-sensitive.cpp"
#include "valid/key/escapes.cpp"
#include "valid/key/special-word.cpp"
#include "valid/newline-crlf.cpp"
#include "valid/spec-example-1-compact.cpp"
#include "valid/float/inf-and-nan.cpp"
#include "valid/float/float.cpp"
#include "valid/float/long.cpp"
#include "valid/float/underscore.cpp"
#include "valid/float/exponent.cpp"
#include "valid/float/zero.cpp"
#include "valid/table/empty.cpp"
#include "valid/table/array-nest.cpp"
#include "valid/table/array-many.cpp"
#include "valid/table/without-super.cpp"
#include "valid/table/keyword.cpp"
#include "valid/table/sub-empty.cpp"
#include "valid/table/with-pound.cpp"
#include "valid/table/no-eol.cpp"
#include "valid/table/names.cpp"
#include "valid/table/whitespace.cpp"
#include "valid/table/with-single-quotes.cpp"
#include "valid/table/array-one.cpp"
#include "valid/table/array-table-array.cpp"
#include "valid/table/array-implicit.cpp"
#include "valid/table/with-literal-string.cpp"
#include "valid/empty-file.cpp"
#include "valid/comment/everywhere.cpp"
#include "valid/comment/at-eof2.cpp"
#include "valid/comment/at-eof.cpp"
#include "valid/comment/tricky.cpp"
#include "valid/comment/noeol.cpp"
#include "valid/bool/bool.cpp"
#include "valid/array/empty.cpp"
#include "valid/array/strings.cpp"
#include "valid/array/nested-inline-table.cpp"
#include "valid/array/string-with-comma.cpp"
#include "valid/array/nospaces.cpp"
#include "valid/array/string-quote-comma-2.cpp"
#include "valid/array/mixed-int-array.cpp"
#include "valid/array/mixed-int-string.cpp"
#include "valid/array/nested-double.cpp"
#include "valid/array/mixed-int-float.cpp"
#include "valid/array/string-quote-comma.cpp"
#include "valid/array/bool.cpp"
#include "valid/array/nested.cpp"
#include "valid/array/mixed-string-table.cpp"
#include "valid/array/array.cpp"
#include "valid/array/table-array-string-backslash.cpp"
#include "valid/array/hetergeneous.cpp"
