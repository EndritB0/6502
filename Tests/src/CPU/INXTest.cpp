#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeINXTest : public CPUTest {};

	TEST_F(OpcodeINXTest, IncrementsTheXRegister)
	{
		cpu.XRegister = 0x41;
		WriteProgram({ Opcode::INX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeINXTest, ConsumesTwoCycles)
	{
		cpu.XRegister = 0x41;
		WriteProgram({ Opcode::INX, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeINXTest, WrapsFromTwoFiftyFiveBackToZero)
	{
		cpu.XRegister = 0xFF;
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::INX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINXTest, LeavesTheCarryFlagClearWhenTheXRegisterWraps)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::INX });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Carry));
	}

	TEST_F(OpcodeINXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x7F;
		WriteProgram({ Opcode::INX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeINXTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.XRegister = 0x41;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::INX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINXTest, LeavesTheOtherFlagsAlone)
	{
		cpu.XRegister = 0x41;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::INX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeINXTest, LeavesTheAccumulatorTheYRegisterAndTheStackPointerAlone)
	{
		cpu.Accumulator = 0x33;
		cpu.XRegister = 0x41;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::INX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeINXTest, IncrementsTheValueTheLoadJustPutIntoTheXRegister)
	{
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x41, Opcode::INX });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINXTest, CountsTheXRegisterUpAcrossRepeatedIncrements)
	{
		cpu.XRegister = 0x40;
		WriteProgram({ Opcode::INX, Opcode::INX, Opcode::INX });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.XRegister, 0x43);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINXTest, AdvancesTheIndexUsedByAZeroPageXLoad)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDX_IMMEDIATE;
		memory[0x4001] = 0x03;
		memory[0x4002] = Opcode::INX;
		memory[0x4003] = Opcode::LDA_ZERO_PAGE_X;
		memory[0x4004] = 0x80;
		memory[0x0084] = 0x37;

		cpu.Execute(3 + 2 + 2 + 4, memory);

		EXPECT_EQ(cpu.XRegister, 0x04);
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4005);
	}

}
