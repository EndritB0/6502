#include "pch.h"

#include "CPU.h"

namespace MOS6502 {

	void Memory::Init()
	{
		for (std::uint32_t i{}; i < Memory::MemorySize; i++)
		{
			Data[i] = 0x00;
		}
	}

	void Memory::WriteWord(Cycles& cycles, Word value, Address address)
	{
		Data[address] = static_cast<Byte>(value & 0x00FF);
		Data[address + 1] = static_cast<Byte>((value & 0xFF00) >> 8);
		cycles -= 2;
	}

}
