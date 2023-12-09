#pragma once
#ifndef SLM1_TYPES
#define SLM1_TYPES

#include <cstddef>
#include <cstdint>

#include "Misc.hpp"

SLM1_NAMESPACE_BEGIN

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using char8 = char;
using char16 = char16_t;
using char32 = char32_t;
using uchar8 = unsigned char;

using byte = unsigned char;

using size = decltype(sizeof(0));

/** TODO: Implement
* Fixed size boolean type
*/
class [[deprecated("This type is still being implemented")]] b8
{
public:
	constexpr inline operator u8()
	{
		return value == 0x00 ? 0x00 : 0xFF;
	}

	constexpr inline b8& operator=(b8 _value)
	{
		value = _value;
		return *this;
	}

	constexpr inline b8& operator=(bool _value)
	{
		value = _value ? 0xFF : 0x00;
		return *this;
	}

private:
	u8 value;
};

SLM1_NAMESPACE_END

#endif // ifndef SLM1_TYPES