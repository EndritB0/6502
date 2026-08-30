#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {
	constexpr Byte PushedStatusBits{ Flag::Break | Flag::Unused };

	constexpr Byte AsPushed(Byte status)
	{
		return static_cast<Byte>(status | PushedStatusBits);
	}

	class OpcodePHPTest : public CPUTest {};

	TEST_F(OpcodePHPTest, PushesTheProcessorStatusOntoTheStack)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], AsPushed(UnrelatedFlags));
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePHPTest, PushesTheBreakAndUnusedBitsSetEvenWhenNoFlagsAre)
	{
		cpu.ProcessorStatus = 0x00;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], PushedStatusBits);
	}

	TEST_F(OpcodePHPTest, LeavesTheBreakAndUnusedBitsOutOfTheStatusRegister)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
		EXPECT_FALSE(cpu.GetFlag(Flag::Break));
		EXPECT_FALSE(cpu.GetFlag(Flag::Unused));
	}

	TEST_F(OpcodePHPTest, MovesTheStackPointerDownOneByte)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer - 1);
	}

	TEST_F(OpcodePHPTest, ConsumesThreeCycles)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::PHP, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], AsPushed(UnrelatedFlags));
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePHPTest, WritesWhereTheStackPointerIsPointing)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		cpu.StackPointer = 0x80;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(0x80)], AsPushed(UnrelatedFlags));
		EXPECT_EQ(cpu.StackPointer, 0x7F);
	}

	TEST_F(OpcodePHPTest, WrapsTheStackPointerRoundWhenTheStackIsFull)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		cpu.StackPointer = 0x00;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(0x00)], AsPushed(UnrelatedFlags));
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodePHPTest, OverwritesTheByteAlreadyOnTheStack)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		memory[StackAddress(InitialStackPointer)] = 0x99;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], AsPushed(UnrelatedFlags));
	}

	TEST_F(OpcodePHPTest, PushesEveryFlagWhenTheyAreAllSet)
	{
		cpu.ProcessorStatus = 0xFF;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], 0xFF);
		EXPECT_EQ(cpu.ProcessorStatus, 0xFF);
	}

	TEST_F(OpcodePHPTest, LeavesTheRegistersAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		cpu.Accumulator = 0x11;
		cpu.XRegister = 0x22;
		cpu.YRegister = 0x33;
		WriteProgram({ Opcode::PHP });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.XRegister, 0x22);
		EXPECT_EQ(cpu.YRegister, 0x33);
	}

	TEST_F(OpcodePHPTest, PushesTheZeroFlagSetByAnEarlierLoad)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x00, Opcode::PHP });

		cpu.Execute(2 + 3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], AsPushed(Flag::Zero));
	}

	TEST_F(OpcodePHPTest, PushesTheNegativeFlagSetByAnEarlierLoad)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x80, Opcode::PHP });

		cpu.Execute(2 + 3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], AsPushed(Flag::Negative));
	}

	TEST_F(OpcodePHPTest, StacksSuccessivePushesDownwards)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::PHP;
		memory[0x4001] = Opcode::LDA_IMMEDIATE;
		memory[0x4002] = 0x00;
		memory[0x4003] = Opcode::PHP;

		cpu.Execute(3 + 3 + 2 + 3, memory);

		EXPECT_EQ(memory[StackAddress(0xFF)], PushedStatusBits);
		EXPECT_EQ(memory[StackAddress(0xFE)], AsPushed(Flag::Zero));
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer - 2);
	}

}
