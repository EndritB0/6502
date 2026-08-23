#include "pch.h"

#include "CPU.h"

namespace MOS6502 {

	void Memory::Init()
	{
		for (std::uint32_t i = 0; i < Memory::MemorySize; ++i)
		{
			Data[i] = 0x00;
		}
	}

	void Memory::WriteWord(std::uint32_t& cycles, std::uint16_t value, std::uint16_t address)
	{
		Data[address] = static_cast<std::uint8_t>(value & 0x00FF);
		Data[address + 1] = static_cast<std::uint8_t>((value & 0xFF00) >> 8);
		cycles -= 2;
	}

}
