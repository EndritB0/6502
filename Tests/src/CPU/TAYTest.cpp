#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeTAYTest : public CPUTest {};

	TEST_F(OpcodeTAYTest, CopiesTheAccumulatorIntoTheYRegister)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTAYTest, ConsumesTwoCycles)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAY, Opcode::LDY_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTAYTest, OverwritesTheValueAlreadyInTheYRegister)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x99;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
	}

	TEST_F(OpcodeTAYTest, SetsTheZeroFlagWhenTheAccumulatorIsZero)
	{
		cpu.Accumulator = 0x00;
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTAYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeTAYTest, ClearsZeroAndNegativeForAPositiveAccumulator)
	{
		cpu.Accumulator = 0x42;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTAYTest, TakesItsFlagsFromTheAccumulatorAndNotTheOverwrittenYRegister)
	{
		cpu.Accumulator = 0x00;
		cpu.YRegister = 0x80;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTAYTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeTAYTest, LeavesTheAccumulatorAlone)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodeTAYTest, LeavesTheXRegisterAndTheStackPointerAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		WriteProgram({ Opcode::TAY });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeTAYTest, CopiesTheValueTheLoadJustPutIntoTheAccumulator)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42, Opcode::TAY });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeTAYTest, HandsTheValueBackToTheAccumulatorThroughTya)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAY, Opcode::LDA_IMMEDIATE, 0x99, Opcode::TYA });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodeTAYTest, PutsAWorkingIndexIntoTheYRegister)
	{
		cpu.Accumulator = 0x04;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::TAY;
		memory[0x4001] = Opcode::LDA_ABSOLUTE_Y;
		memory[0x4002] = 0x00;
		memory[0x4003] = 0x41;
		memory[0x4104] = 0x37;

		cpu.Execute(3 + 2 + 4, memory);

		EXPECT_EQ(cpu.YRegister, 0x04);
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4004);
	}

}
