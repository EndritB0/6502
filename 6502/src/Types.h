#pragma once

#include <cstdint>

namespace MOS6502 {

	using Byte = std::uint8_t;
	using SignedByte = std::int8_t;
	using Word = std::uint16_t;

	using Address = Word;
	using Cycles = std::int32_t;

}
