#pragma once

#include <gtest/gtest.h>

#include <CPU.h>

namespace Test6502 {

	using MOS6502::CPU;
	using MOS6502::Memory;

	using MOS6502::Address;
	using MOS6502::Byte;
	using MOS6502::Cycles;
	using MOS6502::Word;

	namespace Flag = MOS6502::Flag;
	namespace Opcode = MOS6502::Opcode;

	constexpr Address ResetVector{ 0xFFFC };
	constexpr Word InitialStackPointer{ 0x0100 };
	constexpr Byte UnrelatedFlags{ Flag::Carry | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow };

	class CPUTest : public ::testing::Test {
	protected:
		void SetUp() override
		{
			cpu.Reset(memory);
		}

		void WriteProgram(std::initializer_list<Byte> bytes)
		{
			Address address{ ResetVector };
			for (const Byte byte : bytes)
			{
				memory[address] = byte;
				address++;
			}
		}

		CPU cpu;
		Memory memory;
	};

}
