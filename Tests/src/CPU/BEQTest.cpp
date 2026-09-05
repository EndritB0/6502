#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte FlagsBesidesZero{ Flag::Carry | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow | Flag::Negative };
	constexpr Byte FlagsIncludingZero{ FlagsBesidesZero | Flag::Zero };

	class OpcodeBEQTest : public BranchTest {};

	TEST_F(OpcodeBEQTest, BranchesForwardWhenTheZeroFlagIsSet)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBEQTest, FallsThroughToTheNextInstructionWhenTheZeroFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBEQTest, IgnoresTheNegativeFlag)
	{
		cpu.SetFlag(Flag::Negative, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBEQTest, BranchesBackwardsWithANegativeOffset)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0xF0 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4072);
	}

	TEST_F(OpcodeBEQTest, LandsOnTheNextInstructionWhenTheOffsetIsZero)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x00, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBEQTest, ReachesTheFurthestOffsetForwards)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(0x4000, { Opcode::BEQ, 0x7F });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4081);
	}

	TEST_F(OpcodeBEQTest, ReachesTheFurthestOffsetBackwards)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x80 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4002);
	}

	TEST_F(OpcodeBEQTest, CountsTheOffsetFromTheAddressAfterTheOperand)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(0x40FE, { Opcode::BEQ, 0x02 });
		memory[0x4102] = Opcode::LDA_IMMEDIATE;
		memory[0x4103] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4102);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBEQTest, WrapsAroundTheTopOfMemory)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(0xFFFD, { Opcode::BEQ, 0x40 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x003F);
	}

	TEST_F(OpcodeBEQTest, ConsumesTwoCyclesWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBEQTest, ConsumesThreeCyclesWhenTheBranchIsTaken)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBEQTest, ConsumesFourCyclesWhenTheBranchCrossesIntoTheNextPage)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(0x40F0, { Opcode::BEQ, 0x0E });
		memory[0x4100] = Opcode::LDA_IMMEDIATE;
		memory[0x4101] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeBEQTest, ConsumesFourCyclesWhenTheBranchCrossesIntoThePreviousPage)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(0x4002, { Opcode::BEQ, 0x80 });
		memory[0x3F84] = Opcode::LDA_IMMEDIATE;
		memory[0x3F85] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x3F84);
	}

	TEST_F(OpcodeBEQTest, DoesNotPayForThePageCrossWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(0x40FD, { Opcode::BEQ, 0x01, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40FF);
	}

	TEST_F(OpcodeBEQTest, CarriesOnAtTheBranchTarget)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBEQTest, LoopsBackOntoItselfWhileTheZeroFlagIsSet)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0xFE });

		cpu.Execute(3 + 3 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart);
	}

	TEST_F(OpcodeBEQTest, LeavesTheFlagsAlone)
	{
		cpu.ProcessorStatus = FlagsIncludingZero;
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, FlagsIncludingZero);
	}

	TEST_F(OpcodeBEQTest, LeavesTheRegistersAndTheStackAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::BEQ, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeBEQTest, BranchesOnTheZeroFlagTheLoadJustSet)
	{
		cpu.Accumulator = 0x42;
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x00, Opcode::BEQ, 0x20 });

		cpu.Execute(2 + 3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBEQTest, FallsThroughAfterALoadOfANonZeroValue)
	{
		cpu.SetFlag(Flag::Zero, true);
		WriteProgramAt(ProgramStart, { Opcode::LDA_IMMEDIATE, 0x37, Opcode::BEQ, 0x20 });

		cpu.Execute(2 + 2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, 0x4084);
	}

	TEST_F(OpcodeBEQTest, BranchesOnceTheDecrementsReachZero)
	{
		cpu.XRegister = 0x02;
		WriteProgramAt(ProgramStart, { Opcode::DEX, Opcode::BEQ, 0x20, Opcode::DEX, Opcode::BEQ, 0x20 });

		cpu.Execute(2 + 2 + 2 + 3, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A6);
	}

}
