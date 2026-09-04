#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeDEXTest : public CPUTest {};

	TEST_F(OpcodeDEXTest, DecrementsTheXRegister)
	{
		cpu.XRegister = 0x43;
		WriteProgram({ Opcode::DEX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeDEXTest, ConsumesTwoCycles)
	{
		cpu.XRegister = 0x43;
		WriteProgram({ Opcode::DEX, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeDEXTest, WrapsFromZeroBackToTwoFiftyFive)
	{
		cpu.XRegister = 0x00;
		cpu.SetFlag(Flag::Zero, true);
		WriteProgram({ Opcode::DEX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0xFF);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeDEXTest, LeavesTheCarryFlagClearWhenTheXRegisterWraps)
	{
		cpu.XRegister = 0x00;
		WriteProgram({ Opcode::DEX });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Carry));
	}

	TEST_F(OpcodeDEXTest, SetsTheZeroFlagWhenTheXRegisterReachesZero)
	{
		cpu.XRegister = 0x01;
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::DEX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDEXTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.XRegister = 0x43;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::DEX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeDEXTest, LeavesTheOtherFlagsAlone)
	{
		cpu.XRegister = 0x43;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::DEX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeDEXTest, LeavesTheAccumulatorTheYRegisterAndTheStackPointerAlone)
	{
		cpu.Accumulator = 0x33;
		cpu.XRegister = 0x43;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::DEX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeDEXTest, DecrementsTheValueTheLoadJustPutIntoTheXRegister)
	{
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x43, Opcode::DEX });

		cpu.Execute(2 + 2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDEXTest, CountsTheXRegisterDownAcrossRepeatedDecrements)
	{
		cpu.XRegister = 0x45;
		WriteProgram({ Opcode::DEX, Opcode::DEX, Opcode::DEX });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeDEXTest, WalksTheIndexBackDownForAZeroPageXLoad)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDX_IMMEDIATE;
		memory[0x4001] = 0x05;
		memory[0x4002] = Opcode::DEX;
		memory[0x4003] = Opcode::LDA_ZERO_PAGE_X;
		memory[0x4004] = 0x80;
		memory[0x0084] = 0x37;

		cpu.Execute(3 + 2 + 2 + 4, memory);

		EXPECT_EQ(cpu.XRegister, 0x04);
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4005);
	}

}
