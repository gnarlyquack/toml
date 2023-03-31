//  This file is part of toml-parser. It is subject to the license terms in the
//  LICENSE file found in the top-level directory of this distribution. No part
//  of this project, including this file, may be copied, modified, propagated,
//  or distributed except according to the terms contained in the LICENSE file.

#ifndef toml_common_hpp
#define toml_common_hpp

#include <cassert>
#include <climits>
#include <limits>
#include <string>


static_assert(CHAR_BIT == 8, "Expected byte to be 8 bits.");
static_assert(std::numeric_limits<float>::is_iec559, "Expected float to be IEEE 754 compliant.");
static_assert(std::numeric_limits<double>::is_iec559, "Expected double to be IEEE 754 compliant.");

using s8  = signed char;
using s16 = short;
using s32 = int;
using s64 = long long;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned;
using u64 = unsigned long long;

using f32 = float;
using f64 = double;

using byte = unsigned char;


static_assert(sizeof(s8) == 1, "Expected s8 to be 1 byte.");
static_assert(sizeof(s16) == 2, "Expected s16 to be 2 bytes.");
static_assert(sizeof(s32) == 4, "Expected s32 to be 4 bytes.");
static_assert(sizeof(s64) == 8, "Expected s64 to be 8 bytes.");

static_assert(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
static_assert(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
static_assert(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
static_assert(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

static_assert(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
static_assert(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");


#define BYTE_MAX UCHAR_MAX

#define INF32 std::numeric_limits<f32>::infinity()
#define INF64 std::numeric_limits<f64>::infinity()

#define NAN32 std::numeric_limits<f32>::quiet_NaN()
#define NAN64 std::numeric_limits<f64>::quiet_NaN()


#define string_to_s64 std::stoll

#define string_to_f64 std::stod


#endif // toml_common_hpp
