#pragma once

#include "Types.h"

namespace MOS6502 {

	struct Memory {
		static constexpr std::uint32_t MemorySize{ 1024 * 64 };
		Byte Data[MemorySize];

		Byte& operator[](Address address) { return Data[address]; }
		Byte operator[](Address address) const { return Data[address]; }

		void Init();
	};

}
