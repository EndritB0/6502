#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeTAXTest : public CPUTest {};

	TEST_F(OpcodeTAXTest, CopiesTheAccumulatorIntoTheXRegister)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTAXTest, ConsumesTwoCycles)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAX, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTAXTest, OverwritesTheValueAlreadyInTheXRegister)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x99;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
	}

	TEST_F(OpcodeTAXTest, SetsTheZeroFlagWhenTheAccumulatorIsZero)
	{
		cpu.Accumulator = 0x00;
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTAXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeTAXTest, ClearsZeroAndNegativeForAPositiveAccumulator)
	{
		cpu.Accumulator = 0x42;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTAXTest, TakesItsFlagsFromTheAccumulatorAndNotTheOverwrittenXRegister)
	{
		cpu.Accumulator = 0x00;
		cpu.XRegister = 0x80;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTAXTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeTAXTest, LeavesTheAccumulatorAlone)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodeTAXTest, LeavesTheYRegisterAndTheStackPointerAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::TAX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeTAXTest, CopiesTheValueTheLoadJustPutIntoTheAccumulator)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42, Opcode::TAX });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeTAXTest, HandsTheValueBackToTheAccumulatorThroughTxa)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TAX, Opcode::LDA_IMMEDIATE, 0x99, Opcode::TXA });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodeTAXTest, PutsAWorkingIndexIntoTheXRegister)
	{
		cpu.Accumulator = 0x04;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::TAX;
		memory[0x4001] = Opcode::LDA_ZERO_PAGE_X;
		memory[0x4002] = 0x80;
		memory[0x0084] = 0x37;

		cpu.Execute(3 + 2 + 4, memory);

		EXPECT_EQ(cpu.XRegister, 0x04);
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4003);
	}

}
