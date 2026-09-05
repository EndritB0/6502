#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte FlagsBesidesZero{ Flag::Carry | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow | Flag::Negative };

	class OpcodeBNETest : public BranchTest {};

	TEST_F(OpcodeBNETest, BranchesForwardWhenTheZeroFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBNETest, FallsThroughToTheNextInstructionWhenTheZeroFlagIsSet)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBNETest, IgnoresTheNegativeFlag)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBNETest, BranchesBackwardsWithANegativeOffset)
	{
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0xF0 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4072);
	}

	TEST_F(OpcodeBNETest, LandsOnTheNextInstructionWhenTheOffsetIsZero)
	{
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x00, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBNETest, ReachesTheFurthestOffsetForwards)
	{
		WriteProgramAt(0x4000, { Opcode::BNE, 0x7F });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4081);
	}

	TEST_F(OpcodeBNETest, ReachesTheFurthestOffsetBackwards)
	{
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x80 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4002);
	}

	TEST_F(OpcodeBNETest, CountsTheOffsetFromTheAddressAfterTheOperand)
	{
		WriteProgramAt(0x40FE, { Opcode::BNE, 0x02 });
		memory[0x4102] = Opcode::LDA_IMMEDIATE;
		memory[0x4103] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4102);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBNETest, WrapsAroundTheTopOfMemory)
	{
		WriteProgramAt(0xFFFD, { Opcode::BNE, 0x40 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x003F);
	}

	TEST_F(OpcodeBNETest, ConsumesTwoCyclesWhenTheBranchIsNotTaken)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBNETest, ConsumesThreeCyclesWhenTheBranchIsTaken)
	{
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBNETest, ConsumesFourCyclesWhenTheBranchCrossesIntoTheNextPage)
	{
		WriteProgramAt(0x40F0, { Opcode::BNE, 0x0E });
		memory[0x4100] = Opcode::LDA_IMMEDIATE;
		memory[0x4101] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeBNETest, ConsumesFourCyclesWhenTheBranchCrossesIntoThePreviousPage)
	{
		WriteProgramAt(0x4002, { Opcode::BNE, 0x80 });
		memory[0x3F84] = Opcode::LDA_IMMEDIATE;
		memory[0x3F85] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x3F84);
	}

	TEST_F(OpcodeBNETest, DoesNotPayForThePageCrossWhenTheBranchIsNotTaken)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(0x40FD, { Opcode::BNE, 0x01, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40FF);
	}

	TEST_F(OpcodeBNETest, CarriesOnAtTheBranchTarget)
	{
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBNETest, LoopsBackOntoItselfWhileTheZeroFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0xFE });

		cpu.Execute(3 + 3 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart);
	}

	TEST_F(OpcodeBNETest, LeavesTheFlagsAlone)
	{
		cpu.ProcessorStatus = FlagsBesidesZero;
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, FlagsBesidesZero);
	}

	TEST_F(OpcodeBNETest, LeavesTheRegistersAndTheStackAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgramAt(ProgramStart, { Opcode::BNE, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeBNETest, BranchesOnTheZeroFlagTheLoadJustCleared)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x37, Opcode::BNE, 0x20 });

		cpu.Execute(2 + 3, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBNETest, FallsThroughAfterALoadOfZero)
	{
		cpu.Accumulator = 0x42;
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x00, Opcode::BNE, 0x20 });

		cpu.Execute(2 + 2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, 0x4084);
	}

	TEST_F(OpcodeBNETest, RunsACountdownLoopUntilTheDecrementReachesZero)
	{
		WriteProgramAt(ProgramStart, { Opcode::LDX_IMMEDIATE, 0x03, Opcode::DEX, Opcode::BNE, 0xFD, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2 + (2 + 3) + (2 + 3) + (2 + 2), memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4085);
	}

}
