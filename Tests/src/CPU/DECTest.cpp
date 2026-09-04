#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeDECZeroPageTest : public CPUTest {};
	class OpcodeDECZeroPageXTest : public CPUTest {};
	class OpcodeDECAbsoluteTest : public CPUTest {};
	class OpcodeDECAbsoluteXTest : public CPUTest {};

	TEST_F(OpcodeDECZeroPageTest, DecrementsTheValueHeldAtTheZeroPageAddress)
	{
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeDECZeroPageTest, ConsumesFiveCycles)
	{
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeDECZeroPageTest, WrapsFromZeroBackToTwoFiftyFive)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x00;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0xFF);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeDECZeroPageTest, LeavesTheCarryFlagClearWhenTheValueWraps)
	{
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x00;

		cpu.Execute(5, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Carry));
	}

	TEST_F(OpcodeDECZeroPageTest, SetsTheZeroFlagWhenTheValueReachesZero)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x01;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDECZeroPageTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDECZeroPageTest, LeavesTheOtherFlagsAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeDECZeroPageTest, LeavesTheRegistersAlone)
	{
		cpu.Accumulator = 0x33;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeDECZeroPageTest, LeavesTheNeighbouringBytesAlone)
	{
		WriteProgram({ Opcode::DEC_ZERO_PAGE, 0x42 });
		memory[0x0041] = 0x43;
		memory[0x0042] = 0x43;
		memory[0x0043] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x0041], 0x43);
		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(memory[0x0043], 0x43);
	}

	TEST_F(OpcodeDECZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x43;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeDECZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::DEC_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x43;
		memory[0x017F] = 0x43;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x007F], 0x42);
		EXPECT_EQ(memory[0x017F], 0x43);
	}

	TEST_F(OpcodeDECZeroPageXTest, ConsumesSixCycles)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ZERO_PAGE_X, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x43;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeDECZeroPageXTest, WrapsFromZeroBackToTwoFiftyFive)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x00;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0xFF);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeDECZeroPageXTest, SetsTheZeroFlagWhenTheValueReachesZero)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x01;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x0047], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDECZeroPageXTest, LeavesTheXRegisterAlone)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x43;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.XRegister, 0x05);
	}

	TEST_F(OpcodeDECAbsoluteTest, DecrementsTheValueHeldAtTheAbsoluteAddress)
	{
		WriteProgram({ Opcode::DEC_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x43;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDECAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		WriteProgram({ Opcode::DEC_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x43;
		memory[0x3412] = 0x43;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x1234], 0x42);
		EXPECT_EQ(memory[0x3412], 0x43);
	}

	TEST_F(OpcodeDECAbsoluteTest, ConsumesSixCycles)
	{
		WriteProgram({ Opcode::DEC_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x43;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDECAbsoluteTest, WrapsFromZeroBackToTwoFiftyFive)
	{
		WriteProgram({ Opcode::DEC_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x00;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0xFF);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeDECAbsoluteTest, SetsTheZeroFlagWhenTheValueReachesZero)
	{
		WriteProgram({ Opcode::DEC_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x01;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x4480], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDECAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x43;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDECAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::DEC_ABSOLUTE_X, 0x02, 0x44 });
		memory[0x4501] = 0x43;
		memory[0x4401] = 0x43;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(memory[0x4401], 0x43);
	}

	TEST_F(OpcodeDECAbsoluteXTest, ConsumesSevenCyclesWhenNoPageIsCrossed)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ABSOLUTE_X, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x43;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDECAbsoluteXTest, ConsumesSevenCyclesWhenAPageIsCrossed)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::DEC_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x43;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDECAbsoluteXTest, ConsumesNoMoreThanSevenCyclesWhenAPageIsCrossed)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::DEC_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x43;

		cpu.Execute(8, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x99);
	}

	TEST_F(OpcodeDECAbsoluteXTest, WrapsFromZeroBackToTwoFiftyFive)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x00;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0xFF);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeDECAbsoluteXTest, SetsTheZeroFlagWhenTheValueReachesZero)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::DEC_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x01;

		cpu.Execute(7, memory);

		EXPECT_EQ(memory[0x4485], 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDECZeroPageTest, DecrementsTheValueTheStoreJustWrote)
	{
		cpu.Accumulator = 0x43;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::STA_ZERO_PAGE;
		memory[0x4001] = 0x42;
		memory[0x4002] = Opcode::DEC_ZERO_PAGE;
		memory[0x4003] = 0x42;

		cpu.Execute(3 + 3 + 5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x43);
		EXPECT_EQ(cpu.ProgramCounter, 0x4004);
	}

	TEST_F(OpcodeDECZeroPageTest, LeavesTheDecrementedValueForALoadToPickUp)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::DEC_ZERO_PAGE;
		memory[0x4001] = 0x42;
		memory[0x4002] = Opcode::LDA_ZERO_PAGE;
		memory[0x4003] = 0x42;
		memory[0x0042] = 0x43;

		cpu.Execute(3 + 5 + 3, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, 0x4004);
	}

	TEST_F(OpcodeDECZeroPageTest, CountsACounterDownAcrossRepeatedDecrements)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::DEC_ZERO_PAGE;
		memory[0x4001] = 0x42;
		memory[0x4002] = Opcode::DEC_ZERO_PAGE;
		memory[0x4003] = 0x42;
		memory[0x4004] = Opcode::DEC_ZERO_PAGE;
		memory[0x4005] = 0x42;
		memory[0x0042] = 0x45;

		cpu.Execute(3 + 5 + 5 + 5, memory);

		EXPECT_EQ(memory[0x0042], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, 0x4006);
	}

}
