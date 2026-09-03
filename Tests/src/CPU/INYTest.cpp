#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeINYTest : public CPUTest {};

	TEST_F(OpcodeINYTest, IncrementsTheYRegister)
	{
		cpu.YRegister = 0x41;
		WriteProgram({ Opcode::INY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeINYTest, ConsumesTwoCycles)
	{
		cpu.YRegister = 0x41;
		WriteProgram({ Opcode::INY, Opcode::LDY_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeINYTest, WrapsFromTwoFiftyFiveBackToZero)
	{
		cpu.YRegister = 0xFF;
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::INY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINYTest, LeavesTheCarryFlagClearWhenTheYRegisterWraps)
	{
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::INY });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Carry));
	}

	TEST_F(OpcodeINYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.YRegister = 0x7F;
		WriteProgram({ Opcode::INY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeINYTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.YRegister = 0x41;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::INY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeINYTest, LeavesTheOtherFlagsAlone)
	{
		cpu.YRegister = 0x41;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::INY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeINYTest, LeavesTheAccumulatorTheXRegisterAndTheStackPointerAlone)
	{
		cpu.Accumulator = 0x33;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x41;
		WriteProgram({ Opcode::INY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeINYTest, IncrementsTheValueTheLoadJustPutIntoTheYRegister)
	{
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x41, Opcode::INY });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINYTest, CountsTheYRegisterUpAcrossRepeatedIncrements)
	{
		cpu.YRegister = 0x40;
		WriteProgram({ Opcode::INY, Opcode::INY, Opcode::INY });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.YRegister, 0x43);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeINYTest, AdvancesTheIndexUsedByAnAbsoluteYLoad)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDY_IMMEDIATE;
		memory[0x4001] = 0x03;
		memory[0x4002] = Opcode::INY;
		memory[0x4003] = Opcode::LDA_ABSOLUTE_Y;
		memory[0x4004] = 0x80;
		memory[0x4005] = 0x44;
		memory[0x4484] = 0x37;

		cpu.Execute(3 + 2 + 2 + 4, memory);

		EXPECT_EQ(cpu.YRegister, 0x04);
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4006);
	}

}
