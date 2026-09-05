#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte FlagsBesidesNegative{ Flag::Carry | Flag::Zero | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow };

	class OpcodeBPLTest : public BranchTest {};

	TEST_F(OpcodeBPLTest, BranchesForwardWhenTheNegativeFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBPLTest, FallsThroughToTheNextInstructionWhenTheNegativeFlagIsSet)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBPLTest, IgnoresTheZeroFlag)
	{
		cpu.SetFlag(Flag::Negative, true);
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBPLTest, BranchesBackwardsWithANegativeOffset)
	{
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0xF0 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4072);
	}

	TEST_F(OpcodeBPLTest, LandsOnTheNextInstructionWhenTheOffsetIsZero)
	{
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x00, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBPLTest, ReachesTheFurthestOffsetForwards)
	{
		WriteProgramAt(0x4000, { Opcode::BPL, 0x7F });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4081);
	}

	TEST_F(OpcodeBPLTest, ReachesTheFurthestOffsetBackwards)
	{
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x80 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4002);
	}

	TEST_F(OpcodeBPLTest, CountsTheOffsetFromTheAddressAfterTheOperand)
	{
		WriteProgramAt(0x40FE, { Opcode::BPL, 0x02 });
		memory[0x4102] = Opcode::LDA_IMMEDIATE;
		memory[0x4103] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4102);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBPLTest, WrapsAroundTheTopOfMemory)
	{
		WriteProgramAt(0xFFFD, { Opcode::BPL, 0x40 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x003F);
	}

	TEST_F(OpcodeBPLTest, ConsumesTwoCyclesWhenTheBranchIsNotTaken)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBPLTest, ConsumesThreeCyclesWhenTheBranchIsTaken)
	{
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBPLTest, ConsumesFourCyclesWhenTheBranchCrossesIntoTheNextPage)
	{
		WriteProgramAt(0x40F0, { Opcode::BPL, 0x0E });
		memory[0x4100] = Opcode::LDA_IMMEDIATE;
		memory[0x4101] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeBPLTest, ConsumesFourCyclesWhenTheBranchCrossesIntoThePreviousPage)
	{
		WriteProgramAt(0x4002, { Opcode::BPL, 0x80 });
		memory[0x3F84] = Opcode::LDA_IMMEDIATE;
		memory[0x3F85] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x3F84);
	}

	TEST_F(OpcodeBPLTest, DoesNotPayForThePageCrossWhenTheBranchIsNotTaken)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(0x40FD, { Opcode::BPL, 0x01, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40FF);
	}

	TEST_F(OpcodeBPLTest, CarriesOnAtTheBranchTarget)
	{
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBPLTest, LoopsBackOntoItselfWhileTheNegativeFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0xFE });

		cpu.Execute(3 + 3 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart);
	}

	TEST_F(OpcodeBPLTest, LeavesTheFlagsAlone)
	{
		cpu.ProcessorStatus = FlagsBesidesNegative;
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, FlagsBesidesNegative);
	}

	TEST_F(OpcodeBPLTest, LeavesTheRegistersAndTheStackAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgramAt(ProgramStart, { Opcode::BPL, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeBPLTest, BranchesOnTheNegativeFlagTheLoadJustCleared)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x37, Opcode::BPL, 0x20 });

		cpu.Execute(2 + 3, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBPLTest, FallsThroughAfterALoadOfANegativeValue)
	{
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x80, Opcode::BPL, 0x20 });

		cpu.Execute(2 + 2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_EQ(cpu.ProgramCounter, 0x4084);
	}

	TEST_F(OpcodeBPLTest, RunsACountdownLoopUntilTheDecrementGoesNegative)
	{
		WriteProgramAt(ProgramStart, { Opcode::LDX_IMMEDIATE, 0x02, Opcode::DEX, Opcode::BPL, 0xFD, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2 + (2 + 3) + (2 + 3) + (2 + 2), memory);

		EXPECT_EQ(cpu.XRegister, 0xFF);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4085);
	}

}
