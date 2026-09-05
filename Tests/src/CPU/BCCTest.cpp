#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte FlagsBesidesCarry{ Flag::Zero | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow | Flag::Negative };

	class OpcodeBCCTest : public BranchTest {};

	TEST_F(OpcodeBCCTest, BranchesForwardWhenTheCarryFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBCCTest, FallsThroughToTheNextInstructionWhenTheCarryFlagIsSet)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBCCTest, BranchesBackwardsWithANegativeOffset)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0xF0 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4072);
	}

	TEST_F(OpcodeBCCTest, LandsOnTheNextInstructionWhenTheOffsetIsZero)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x00, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBCCTest, ReachesTheFurthestOffsetForwards)
	{
		WriteProgramAt(0x4000, { Opcode::BCC, 0x7F });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4081);
	}

	TEST_F(OpcodeBCCTest, ReachesTheFurthestOffsetBackwards)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x80 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4002);
	}

	TEST_F(OpcodeBCCTest, CountsTheOffsetFromTheAddressAfterTheOperand)
	{
		WriteProgramAt(0x40FE, { Opcode::BCC, 0x02 });
		memory[0x4102] = Opcode::LDA_IMMEDIATE;
		memory[0x4103] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4102);
		EXPECT_EQ(cpu.Accumulator, 0x00);
	}

	TEST_F(OpcodeBCCTest, WrapsAroundTheTopOfMemory)
	{
		WriteProgramAt(0xFFFD, { Opcode::BCC, 0x40 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x003F);
	}

	TEST_F(OpcodeBCCTest, ConsumesTwoCyclesWhenTheBranchIsNotTaken)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ProgramStart + 2);
	}

	TEST_F(OpcodeBCCTest, ConsumesThreeCyclesWhenTheBranchIsTaken)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A2);
	}

	TEST_F(OpcodeBCCTest, ConsumesFourCyclesWhenTheBranchCrossesIntoTheNextPage)
	{
		WriteProgramAt(0x40F0, { Opcode::BCC, 0x0E });
		memory[0x4100] = Opcode::LDA_IMMEDIATE;
		memory[0x4101] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeBCCTest, ConsumesFourCyclesWhenTheBranchCrossesIntoThePreviousPage)
	{
		WriteProgramAt(0x4002, { Opcode::BCC, 0x80 });
		memory[0x3F84] = Opcode::LDA_IMMEDIATE;
		memory[0x3F85] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x3F84);
	}

	TEST_F(OpcodeBCCTest, DoesNotPayForThePageCrossWhenTheBranchIsNotTaken)
	{
		cpu.SetFlag(Flag::Carry, true);
		WriteProgramAt(0x40FD, { Opcode::BCC, 0x01, Opcode::LDA_IMMEDIATE, 0x37 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x40FF);
	}

	TEST_F(OpcodeBCCTest, CarriesOnAtTheBranchTarget)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20 });
		memory[0x40A2] = Opcode::LDA_IMMEDIATE;
		memory[0x40A3] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x40A4);
	}

	TEST_F(OpcodeBCCTest, LoopsBackOntoItselfWhileTheCarryFlagIsClear)
	{
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0xFE });

		cpu.Execute(3 + 3 + 3, memory);

		EXPECT_EQ(cpu.ProgramCounter, ProgramStart);
	}

	TEST_F(OpcodeBCCTest, LeavesTheFlagsAlone)
	{
		cpu.ProcessorStatus = FlagsBesidesCarry;
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, FlagsBesidesCarry);
	}

	TEST_F(OpcodeBCCTest, LeavesTheRegistersAndTheStackAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgramAt(ProgramStart, { Opcode::BCC, 0x20 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeBCCTest, BranchesOnTheCarryFlagThePullJustRestored)
	{
		cpu.SetFlag(Flag::Carry, true);
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = Flag::Zero;
		WriteProgramAt(ProgramStart, { Opcode::PLP, Opcode::BCC, 0x20 });

		cpu.Execute(4 + 3, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Carry));
		EXPECT_EQ(cpu.ProgramCounter, 0x40A3);
	}

}
