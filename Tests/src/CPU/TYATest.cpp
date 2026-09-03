#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeTYATest : public CPUTest {};

	TEST_F(OpcodeTYATest, CopiesTheYRegisterIntoTheAccumulator)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTYATest, ConsumesTwoCycles)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::TYA, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTYATest, OverwritesTheValueAlreadyInTheAccumulator)
	{
		cpu.YRegister = 0x42;
		cpu.Accumulator = 0x99;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodeTYATest, SetsTheZeroFlagWhenTheYRegisterIsZero)
	{
		cpu.YRegister = 0x00;
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTYATest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.YRegister = 0x80;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeTYATest, ClearsZeroAndNegativeForAPositiveYRegister)
	{
		cpu.YRegister = 0x42;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTYATest, TakesItsFlagsFromTheYRegisterAndNotTheOverwrittenAccumulator)
	{
		cpu.YRegister = 0x00;
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTYATest, LeavesTheOtherFlagsAlone)
	{
		cpu.YRegister = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeTYATest, LeavesTheYRegisterAlone)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
	}

	TEST_F(OpcodeTYATest, LeavesTheXRegisterAndTheStackPointerAlone)
	{
		cpu.YRegister = 0x42;
		cpu.XRegister = 0x11;
		WriteProgram({ Opcode::TYA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeTYATest, CopiesTheValueTheLoadJustPutIntoTheYRegister)
	{
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x42, Opcode::TYA });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeTYATest, HandsTheValueBackToTheYRegisterThroughTay)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::TYA, Opcode::LDY_IMMEDIATE, 0x99, Opcode::TAY });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.YRegister, 0x42);
	}

	TEST_F(OpcodeTYATest, PutsAValueTheStoreCanWriteIntoTheAccumulator)
	{
		cpu.YRegister = 0x37;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::TYA;
		memory[0x4001] = Opcode::STA_ZERO_PAGE;
		memory[0x4002] = 0x80;

		cpu.Execute(3 + 2 + 3, memory);

		EXPECT_EQ(memory[0x0080], 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4003);
	}

}
