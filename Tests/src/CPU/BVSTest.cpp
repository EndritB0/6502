#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte FlagsBesidesOverflow{ Flag::Carry | Flag::Zero | Flag::InterruptDisable | Flag::Decimal | Flag::Negative };
	constexpr Byte FlagsIncludingOverflow{ FlagsBesidesOverflow | Flag::Overflow };

	class OpcodeBVSTest : public BranchTest {};

	TEST_F(OpcodeBVSTest, BranchesForwardWhenTheOverflowFlagIsSet)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBVSTest, FallsThroughToTheNextInstructionWhenTheOverflowFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBVSTest, IgnoresTheNegativeFlag)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBVSTest, BranchesBackwardsWithANegativeOffset)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0xF0 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4072);
	}

	TEST_F(OpcodeBVSTest, LandsOnTheNextInstructionWhenTheOffsetIsZero)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x00, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBVSTest, ReachesTheFurthestOffsetForwards)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(0x4000, { Opcode::BVS, 0x7F });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4081);
	}

	TEST_F(OpcodeBVSTest, ReachesTheFurthestOffsetBackwards)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x80 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4002);
	}

	TEST_F(OpcodeBVSTest, CountsTheOffsetFromTheAddressAfterTheOperand)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(0x40FE, { Opcode::BVS, 0x02 });
		memory[0x4102] = Opcode::LDA_IMMEDIATE;
		memory[0x4103] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4102);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBVSTest, WrapsAroundTheTopOfMemory)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(0xFFFD, { Opcode::BVS, 0x40 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x003F);
	}

	TEST_F(OpcodeBVSTest, ConsumesTwoCyclesWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBVSTest, ConsumesThreeCyclesWhenTheBranchIsTaken)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBVSTest, ConsumesFourCyclesWhenTheBranchCrossesIntoTheNextPage)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(0x40F0, { Opcode::BVS, 0x0E });
		memory[0x4100] = Opcode::LDA_IMMEDIATE;
		memory[0x4101] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeBVSTest, ConsumesFourCyclesWhenTheBranchCrossesIntoThePreviousPage)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(0x4002, { Opcode::BVS, 0x80 });
		memory[0x3F84] = Opcode::LDA_IMMEDIATE;
		memory[0x3F85] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x3F84);
	}

	TEST_F(OpcodeBVSTest, DoesNotPayForThePageCrossWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(0x40FD, { Opcode::BVS, 0x01, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40FF);
	}

	TEST_F(OpcodeBVSTest, CarriesOnAtTheBranchTarget)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBVSTest, LoopsBackOntoItselfWhileTheOverflowFlagIsSet)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0xFE });

		cpu.Execute(3 + 3 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart);
	}

	TEST_F(OpcodeBVSTest, LeavesTheFlagsAlone)
	{
		cpu.ProcessorStatus = FlagsIncludingOverflow;
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, FlagsIncludingOverflow);
	}

	TEST_F(OpcodeBVSTest, LeavesTheRegistersAndTheStackAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BVS, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeBVSTest, BranchesOnTheOverflowFlagTheBitTestJustSet)
	{
		WriteProgramAt(ProgramStart, { Opcode::BIT_ZERO_PAGE, 0x42, Opcode::BVS, 0x20 });
		memory[0x0042] = 0x40;

		cpu.Execute(3 + 3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBVSTest, FallsThroughAfterABitTestOfAValueWithBitSixClear)
	{
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgramAt(ProgramStart, { Opcode::BIT_ZERO_PAGE, 0x42, Opcode::BVS, 0x20 });
		memory[0x0042] = 0x00;

		cpu.Execute(3 + 2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
		EXPECT_EQ(cpu.ProgramCounter, 0x4084);
	}

	TEST_F(OpcodeBVSTest, IsNotFooledByBitSevenOfTheTestedValue)
	{
		WriteProgramAt(ProgramStart, { Opcode::BIT_ZERO_PAGE, 0x42, Opcode::BVS, 0x20 });
		memory[0x0042] = 0x80;

		cpu.Execute(3 + 2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
		EXPECT_EQ(cpu.ProgramCounter, 0x4084);
	}

}
