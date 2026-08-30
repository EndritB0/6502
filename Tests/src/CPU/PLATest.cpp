#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodePLATest : public CPUTest {};

	TEST_F(OpcodePLATest, PullsTheByteFromTheStackIntoTheAccumulator)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePLATest, MovesTheStackPointerBackUpOneByte)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodePLATest, ConsumesFourCycles)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x42;
		WriteProgram({ Opcode::PLA, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePLATest, ReadsFromWhereTheStackPointerIsPointing)
	{
		cpu.StackPointer = 0x7F;
		memory[StackAddress(0x80)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.StackPointer, 0x80);
	}

	TEST_F(OpcodePLATest, WrapsTheStackPointerRoundWhenTheStackIsEmpty)
	{
		cpu.StackPointer = InitialStackPointer;
		memory[StackAddress(0x00)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.StackPointer, 0x00);
	}

	TEST_F(OpcodePLATest, LeavesTheByteBehindOnTheStack)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], 0x42);
	}

	TEST_F(OpcodePLATest, SetsTheZeroFlagWhenThePulledByteIsZero)
	{
		cpu.Accumulator = 0x42;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x00;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodePLATest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x80;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodePLATest, ClearsZeroAndNegativeForAPositiveByte)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodePLATest, LeavesTheOtherFlagsAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodePLATest, LeavesTheOtherRegistersAlone)
	{
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x42;
		WriteProgram({ Opcode::PLA });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodePLATest, BringsBackTheValuePushedByPha)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDA_IMMEDIATE;
		memory[0x4001] = 0x42;
		memory[0x4002] = Opcode::PHA;
		memory[0x4003] = Opcode::LDA_IMMEDIATE;
		memory[0x4004] = 0x99;
		memory[0x4005] = Opcode::PLA;

		cpu.Execute(3 + 2 + 3 + 2 + 4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodePLATest, UnwindsSeveralPushesInReverseOrder)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 2);
		memory[StackAddress(0xFE)] = 0x22;
		memory[StackAddress(0xFF)] = 0x11;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::PLA;
		memory[0x4001] = Opcode::STA_ZERO_PAGE;
		memory[0x4002] = 0x10;
		memory[0x4003] = Opcode::PLA;
		memory[0x4004] = Opcode::STA_ZERO_PAGE;
		memory[0x4005] = 0x11;

		cpu.Execute(3 + 4 + 3 + 4 + 3, memory);

		EXPECT_EQ(memory[0x0010], 0x22);
		EXPECT_EQ(memory[0x0011], 0x11);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

}
