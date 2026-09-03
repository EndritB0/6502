#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeINCZeroPageTest : public CPUTest {};
	class OpcodeINCZeroPageXTest : public CPUTest {};
	class OpcodeINCAbsoluteTest : public CPUTest {};
	class OpcodeINCAbsoluteXTest : public CPUTest {};

	TEST_F(OpcodeINCZeroPageTest, IncrementsTheValueHeldAtTheZeroPageAddress)
	{
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x41;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeINCZeroPageTest, ConsumesFiveCycles)
	{
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x41;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeINCZeroPageTest, WrapsFromTwoFiftyFiveBackToZero)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINCZeroPageTest, LeavesTheCarryFlagClearWhenTheValueWraps)
	{
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Carry));
	}

	TEST_F(OpcodeINCZeroPageTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x7F;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeINCZeroPageTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x41;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINCZeroPageTest, LeavesTheOtherFlagsAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x41;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeINCZeroPageTest, LeavesTheRegistersAlone)
	{
		cpu.Accumulator = 0x33;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x41;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeINCZeroPageTest, LeavesTheNeighbouringBytesAlone)
	{
		WriteProgram({ Opcode::INC_ZERO_PAGE, 0x42 });
		memory[0x0041] = 0x41;
		memory[0x0042] = 0x41;
		memory[0x0043] = 0x41;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0041], 0x41);
		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(memory[0x0043], 0x41);
	}

	TEST_F(OpcodeINCZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x41;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeINCZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::INC_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x41;
		memory[0x017F] = 0x41;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x007F], 0x42);
		EXPECT_EQ(memory[0x017F], 0x41);
	}

	TEST_F(OpcodeINCZeroPageXTest, ConsumesSixCycles)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ZERO_PAGE_X, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x41;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeINCZeroPageXTest, WrapsFromTwoFiftyFiveBackToZero)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINCZeroPageXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x7F;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeINCZeroPageXTest, LeavesTheXRegisterAlone)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x41;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.XRegister, 0x05);
	}

	TEST_F(OpcodeINCAbsoluteTest, IncrementsTheValueHeldAtTheAbsoluteAddress)
	{
		WriteProgram({ Opcode::INC_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x41;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINCAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		WriteProgram({ Opcode::INC_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x41;
		memory[0x3412] = 0x41;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x1234], 0x42);
		EXPECT_EQ(memory[0x3412], 0x41);
	}

	TEST_F(OpcodeINCAbsoluteTest, ConsumesSixCycles)
	{
		WriteProgram({ Opcode::INC_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x41;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINCAbsoluteTest, WrapsFromTwoFiftyFiveBackToZero)
	{
		WriteProgram({ Opcode::INC_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINCAbsoluteTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::INC_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x7F;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeINCAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x41;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINCAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::INC_ABSOLUTE_X, 0x02, 0x44 });
		memory[0x4501] = 0x41;
		memory[0x4401] = 0x41;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(memory[0x4401], 0x41);
	}

	TEST_F(OpcodeINCAbsoluteXTest, ConsumesSevenCyclesWhenNoPageIsCrossed)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ABSOLUTE_X, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x41;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINCAbsoluteXTest, ConsumesSevenCyclesWhenAPageIsCrossed)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::INC_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x41;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINCAbsoluteXTest, WrapsFromTwoFiftyFiveBackToZero)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0xFF;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINCAbsoluteXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::INC_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x7F;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeINCZeroPageTest, IncrementsTheValueTheStoreJustWrote)
	{
		cpu.Accumulator = 0x41;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::STA_ZERO_PAGE;
		memory[0x4001] = 0x42;
		memory[0x4002] = Opcode::INC_ZERO_PAGE;
		memory[0x4003] = 0x42;

		cpu.Execute(3 + 3 + 5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x41);
		EXPECT_EQ(cpu.ProgramCounter, 0x4004);
	}

	TEST_F(OpcodeINCZeroPageTest, LeavesTheIncrementedValueForALoadToPickUp)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::INC_ZERO_PAGE;
		memory[0x4001] = 0x42;
		memory[0x4002] = Opcode::LDA_ZERO_PAGE;
		memory[0x4003] = 0x42;
		memory[0x0042] = 0x41;

		cpu.Execute(3 + 5 + 3, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, 0x4004);
	}

	TEST_F(OpcodeINCZeroPageTest, CountsACounterUpAcrossRepeatedIncrements)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::INC_ZERO_PAGE;
		memory[0x4001] = 0x42;
		memory[0x4002] = Opcode::INC_ZERO_PAGE;
		memory[0x4003] = 0x42;
		memory[0x4004] = Opcode::INC_ZERO_PAGE;
		memory[0x4005] = 0x42;
		memory[0x0042] = 0x40;

		cpu.Execute(3 + 5 + 5 + 5, memory);

		EXPECT_EQ(memory[0x0042], 0x43);
		EXPECT_EQ(cpu.ProgramCounter, 0x4006);
	}

}
