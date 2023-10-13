// This file is part of toml. It is subject to the license terms in the LICENSE
// file found in the top-level directory of this distribution. No part of this
// project, including this file, may be copied, modified, propagated, or
// distributed except according to the terms contained in the LICENSE file.

#ifndef toml_common_hpp
#define toml_common_hpp

#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>


static_assert(CHAR_BIT == 8, "Expected byte to be 8 bits.");
static_assert(std::numeric_limits<float>::is_iec559, "Expected float to be IEEE 754 compliant.");
static_assert(std::numeric_limits<double>::is_iec559, "Expected double to be IEEE 754 compliant.");

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using byte = unsigned char;


#define BYTE_MAX UCHAR_MAX

#define INF32 std::numeric_limits<f32>::infinity()
#define INF64 std::numeric_limits<f64>::infinity()

#define NAN32 std::numeric_limits<f32>::quiet_NaN()
#define NAN64 std::numeric_limits<f64>::quiet_NaN()


#endif // toml_common_hpp
