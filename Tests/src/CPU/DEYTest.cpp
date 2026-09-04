#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeDEYTest : public CPUTest {};

	TEST_F(OpcodeDEYTest, DecrementsTheYRegister)
	{
		cpu.YRegister = 0x43;
		WriteProgram({ Opcode::DEY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeDEYTest, ConsumesTwoCycles)
	{
		cpu.YRegister = 0x43;
		WriteProgram({ Opcode::DEY, Opcode::LDY_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeDEYTest, WrapsFromZeroBackToTwoFiftyFive)
	{
		cpu.YRegister = 0x00;
		cpu.SetFlag(Flag::Zero, true);
		WriteProgram({ Opcode::DEY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0xFF);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeDEYTest, LeavesTheCarryFlagClearWhenTheYRegisterWraps)
	{
		cpu.YRegister = 0x00;
		WriteProgram({ Opcode::DEY });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Carry));
	}

	TEST_F(OpcodeDEYTest, SetsTheZeroFlagWhenTheYRegisterReachesZero)
	{
		cpu.YRegister = 0x01;
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::DEY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDEYTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.YRegister = 0x43;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::DEY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDEYTest, LeavesTheOtherFlagsAlone)
	{
		cpu.YRegister = 0x43;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::DEY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeDEYTest, LeavesTheAccumulatorTheXRegisterAndTheStackPointerAlone)
	{
		cpu.Accumulator = 0x33;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x43;
		WriteProgram({ Opcode::DEY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeDEYTest, DecrementsTheValueTheLoadJustPutIntoTheYRegister)
	{
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x43, Opcode::DEY });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDEYTest, CountsTheYRegisterDownAcrossRepeatedDecrements)
	{
		cpu.YRegister = 0x45;
		WriteProgram({ Opcode::DEY, Opcode::DEY, Opcode::DEY });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDEYTest, WalksTheIndexBackDownForAnAbsoluteYLoad)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDY_IMMEDIATE;
		memory[0x4001] = 0x05;
		memory[0x4002] = Opcode::DEY;
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
