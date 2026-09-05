#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte FlagsBesidesCarry{ Flag::Zero | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow | Flag::Negative };
	constexpr Byte FlagsIncludingCarry{ FlagsBesidesCarry | Flag::Carry };

	class OpcodeBCSTest : public BranchTest {};

	TEST_F(OpcodeBCSTest, BranchesForwardWhenTheCarryFlagIsSet)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBCSTest, FallsThroughToTheNextInstructionWhenTheCarryFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBCSTest, BranchesBackwardsWithANegativeOffset)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0xF0 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4072);
	}

	TEST_F(OpcodeBCSTest, LandsOnTheNextInstructionWhenTheOffsetIsZero)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x00, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBCSTest, ReachesTheFurthestOffsetForwards)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(0x4000, { Opcode::BCS, 0x7F });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4081);
	}

	TEST_F(OpcodeBCSTest, ReachesTheFurthestOffsetBackwards)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x80 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4002);
	}

	TEST_F(OpcodeBCSTest, CountsTheOffsetFromTheAddressAfterTheOperand)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(0x40FE, { Opcode::BCS, 0x02 });
		memory[0x4102] = Opcode::LDA_IMMEDIATE;
		memory[0x4103] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4102);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBCSTest, WrapsAroundTheTopOfMemory)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(0xFFFD, { Opcode::BCS, 0x40 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x003F);
	}

	TEST_F(OpcodeBCSTest, ConsumesTwoCyclesWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x20, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBCSTest, ConsumesThreeCyclesWhenTheBranchIsTaken)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBCSTest, ConsumesFourCyclesWhenTheBranchCrossesIntoTheNextPage)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(0x40F0, { Opcode::BCS, 0x0E });
		memory[0x4100] = Opcode::LDA_IMMEDIATE;
		memory[0x4101] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeBCSTest, ConsumesFourCyclesWhenTheBranchCrossesIntoThePreviousPage)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(0x4002, { Opcode::BCS, 0x80 });
		memory[0x3F84] = Opcode::LDA_IMMEDIATE;
		memory[0x3F85] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x3F84);
	}

	TEST_F(OpcodeBCSTest, DoesNotPayForThePageCrossWhenTheBranchIsNotTaken)
	{
		WriteProgramAt(0x40FD, { Opcode::BCS, 0x01, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40FF);
	}

	TEST_F(OpcodeBCSTest, CarriesOnAtTheBranchTarget)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBCSTest, LoopsBackOntoItselfWhileTheCarryFlagIsSet)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0xFE });

		cpu.Execute(3 + 3 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart);
	}

	TEST_F(OpcodeBCSTest, LeavesTheFlagsAlone)
	{
		cpu.ProcessorStatus = FlagsIncludingCarry;
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, FlagsIncludingCarry);
	}

	TEST_F(OpcodeBCSTest, LeavesTheRegistersAndTheStackAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCS, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeBCSTest, BranchesOnTheCarryFlagThePullJustRestored)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = Flag::Carry;
		WriteProgramAt(ProgramStart, { Opcode::PLP, Opcode::BCS, 0x20 });

		cpu.Execute(4 + 3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Carry));
		EXPECT_EQ(cpu.ProgramCounter, 0x40A3);
	}

	TEST_F(OpcodeBCSTest, TakesTheOppositeBranchToBCCOnTheSameFlag)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20, Opcode::BCS, 0x20 });

		cpu.Execute(2 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

}
