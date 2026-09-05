#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte FlagsBesidesNegative{ Flag::Carry | Flag::Zero | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow };
	constexpr Byte FlagsIncludingNegative{ FlagsBesidesNegative | Flag::Negative };

	class OpcodeBMITest : public BranchTest {};

	TEST_F(OpcodeBMITest, BranchesForwardWhenTheNegativeFlagIsSet)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBMITest, FallsThroughToTheNextInstructionWhenTheNegativeFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBMITest, IgnoresTheZeroFlag)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBMITest, BranchesBackwardsWithANegativeOffset)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0xF0 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4072);
	}

	TEST_F(OpcodeBMITest, LandsOnTheNextInstructionWhenTheOffsetIsZero)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x00, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBMITest, ReachesTheFurthestOffsetForwards)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(0x4000, { Opcode::BMI, 0x7F });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4081);
	}

	TEST_F(OpcodeBMITest, ReachesTheFurthestOffsetBackwards)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x80 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4002);
	}

	TEST_F(OpcodeBMITest, CountsTheOffsetFromTheAddressAfterTheOperand)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(0x40FE, { Opcode::BMI, 0x02 });
		memory[0x4102] = Opcode::LDA_IMMEDIATE;
		memory[0x4103] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4102);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBMITest, WrapsAroundTheTopOfMemory)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(0xFFFD, { Opcode::BMI, 0x40 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x003F);
	}

	TEST_F(OpcodeBMITest, ConsumesTwoCyclesWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBMITest, ConsumesThreeCyclesWhenTheBranchIsTaken)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBMITest, ConsumesFourCyclesWhenTheBranchCrossesIntoTheNextPage)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(0x40F0, { Opcode::BMI, 0x0E });
		memory[0x4100] = Opcode::LDA_IMMEDIATE;
		memory[0x4101] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeBMITest, ConsumesFourCyclesWhenTheBranchCrossesIntoThePreviousPage)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(0x4002, { Opcode::BMI, 0x80 });
		memory[0x3F84] = Opcode::LDA_IMMEDIATE;
		memory[0x3F85] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x3F84);
	}

	TEST_F(OpcodeBMITest, DoesNotPayForThePageCrossWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(0x40FD, { Opcode::BMI, 0x01, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40FF);
	}

	TEST_F(OpcodeBMITest, CarriesOnAtTheBranchTarget)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBMITest, LoopsBackOntoItselfWhileTheNegativeFlagIsSet)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0xFE });

		cpu.Execute(3 + 3 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart);
	}

	TEST_F(OpcodeBMITest, LeavesTheFlagsAlone)
	{
		cpu.ProcessorStatus = FlagsIncludingNegative;
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, FlagsIncludingNegative);
	}

	TEST_F(OpcodeBMITest, LeavesTheRegistersAndTheStackAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BMI, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeBMITest, BranchesOnTheNegativeFlagTheLoadJustSet)
	{
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x80, Opcode::BMI, 0x20 });

		cpu.Execute(2 + 3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBMITest, FallsThroughAfterALoadOfAPositiveValue)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x37, Opcode::BMI, 0x20 });

		cpu.Execute(2 + 2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_EQ(cpu.ProgramCounter, 0x4084);
	}

	TEST_F(OpcodeBMITest, BranchesOnTheNegativeFlagTheDecrementJustSet)
	{
		cpu.XRegister = 0x00;
		WriteProgramAt(ProgramStart, { Opcode::DEX, Opcode::BMI, 0x20 });

		cpu.Execute(2 + 3, memory);

		EXPECT_EQ(cpu.XRegister, 0xFF);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A3);
	}

}
