#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeTXATest : public CPUTest {};

	TEST_F(OpcodeTXATest, CopiesTheXRegisterIntoTheAccumulator)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTXATest, ConsumesTwoCycles)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXA, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTXATest, OverwritesTheValueAlreadyInTheAccumulator)
	{
		cpu.XRegister = 0x42;
		cpu.Accumulator = 0x99;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodeTXATest, SetsTheZeroFlagWhenTheXRegisterIsZero)
	{
		cpu.XRegister = 0x00;
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTXATest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x80;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeTXATest, ClearsZeroAndNegativeForAPositiveXRegister)
	{
		cpu.XRegister = 0x42;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTXATest, TakesItsFlagsFromTheXRegisterAndNotTheOverwrittenAccumulator)
	{
		cpu.XRegister = 0x00;
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTXATest, LeavesTheOtherFlagsAlone)
	{
		cpu.XRegister = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeTXATest, LeavesTheXRegisterAlone)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
	}

	TEST_F(OpcodeTXATest, LeavesTheStackPointerAloneUnlikeTxs)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::TXA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeTXATest, CopiesTheValueTheLoadJustPutIntoTheXRegister)
	{
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x42, Opcode::TXA });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeTXATest, HandsTheValueBackToTheXRegisterThroughTax)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXA, Opcode::LDX_IMMEDIATE, 0x99, Opcode::TAX });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
	}

	TEST_F(OpcodeTXATest, PutsAValueTheStoreCanWriteIntoTheAccumulator)
	{
		cpu.XRegister = 0x37;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::TXA;
		memory[0x4001] = Opcode::STA_ZERO_PAGE;
		memory[0x4002] = 0x80;

		cpu.Execute(3 + 2 + 3, memory);

		EXPECT_EQ(memory[0x0080], 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4003);
	}

}
