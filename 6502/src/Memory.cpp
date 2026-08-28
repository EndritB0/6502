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

}
