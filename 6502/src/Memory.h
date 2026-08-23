#pragma once

#include <cstdint>

namespace MOS6502 {

	struct Memory {
		static constexpr std::uint32_t MemorySize{ 1024 * 64 };
		std::uint8_t Data[MemorySize];

		std::uint8_t& operator[](std::uint32_t address) { return Data[address]; }
		std::uint8_t operator[](std::uint32_t address) const { return Data[address]; }

		void Init();
		void WriteWord(std::uint32_t& cycles, std::uint16_t value, std::uint16_t address);
	};

}
