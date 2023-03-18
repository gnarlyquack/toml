//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef toml_common_hpp
#define toml_common_hpp

#include <cassert>
#include <climits>


static_assert(CHAR_BIT == 8, "Byte must be 8 bits wide");


using byte = unsigned char;

#define BYTE_MAX UCHAR_MAX


#endif // toml_common_hpp
