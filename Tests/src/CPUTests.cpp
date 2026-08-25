#include "pch.h"

#include <CPU.h>

#include <cstdint>
#include <initializer_list>

namespace Test6502 {

	using MOS6502::CPU;
	using MOS6502::Memory;
	namespace Flag = MOS6502::Flag;
	namespace Opcode = MOS6502::Opcode;

	constexpr std::uint16_t ResetVector{ 0xFFFC };
	constexpr std::uint16_t InitialStackPointer{ 0x0100 };
	constexpr std::uint8_t UnrelatedFlags{ Flag::Carry | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow };

	class CPUTest : public ::testing::Test {
	protected:
		void SetUp() override
		{
			cpu.Reset(memory);
		}

		void WriteProgram(std::initializer_list<std::uint8_t> bytes)
		{
			std::uint16_t address{ ResetVector };
			for (const std::uint8_t byte : bytes)
			{
				memory[address] = byte;
				address++;
			}
		}

		CPU cpu;
		Memory memory;
	};

	class OpcodeLDAImmediateTest : public CPUTest {};
	class OpcodeLDAZeroPageTest : public CPUTest {};
	class OpcodeLDAZeroPageXTest : public CPUTest {};
	class OpcodeJSRTest : public CPUTest {};

	TEST_F(OpcodeLDAImmediateTest, LoadsTheOperandIntoTheAccumulator)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAImmediateTest, ConsumesTwoCycles)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAImmediateTest, SetsTheZeroFlagWhenTheOperandIsZero)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x00 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAImmediateTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x80 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAImmediateTest, ClearsZeroAndNegativeForAPositiveOperand)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAImmediateTest, LeavesTheOtherFlagsAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeLDAImmediateTest, LeavesTheIndexRegistersAlone)
	{
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeLDAZeroPageTest, LoadsTheValueHeldAtTheZeroPageAddress)
	{
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageTest, ConsumesThreeCycles)
	{
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x00;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAZeroPageTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xF0;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0xF0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x37;
		memory[0x017F] = 0x99;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAZeroPageXTest, ConsumesFourCycles)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageXTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAZeroPageXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeJSRTest, JumpsToTheSubroutineAddress)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4242);
	}

	TEST_F(OpcodeJSRTest, SavesTheAddressOfTheLastInstructionByteLittleEndian)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[InitialStackPointer], 0xFE);
		EXPECT_EQ(memory[InitialStackPointer + 1], 0xFF);
	}

	TEST_F(OpcodeJSRTest, MovesTheStackPointerPastTheSavedAddress)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer + 2);
	}

	TEST_F(OpcodeJSRTest, LeavesTheRegistersAndFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeJSRTest, ConsumesSixCyclesAndCarriesOnIntoTheSubroutine)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });
		memory[0x4242] = Opcode::LDA_IMMEDIATE;
		memory[0x4243] = 0x37;

		cpu.Execute(6 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4244);
	}

}
