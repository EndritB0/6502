#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte IgnoredStatusBits{ Flag::Break | Flag::Unused };

	constexpr Byte AsPulled(Byte status)
	{
		return static_cast<Byte>(status & ~IgnoredStatusBits);
	}

	class OpcodePLPTest : public CPUTest {};

	TEST_F(OpcodePLPTest, PullsTheStatusByteFromTheStack)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = UnrelatedFlags;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePLPTest, IgnoresTheBreakAndUnusedBitsOnTheWayIn)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0xFF;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, AsPulled(0xFF));
		EXPECT_FALSE(cpu.GetFlag(Flag::Break));
		EXPECT_FALSE(cpu.GetFlag(Flag::Unused));
	}

	TEST_F(OpcodePLPTest, MovesTheStackPointerBackUpOneByte)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = UnrelatedFlags;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodePLPTest, ConsumesFourCycles)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = UnrelatedFlags;
		WriteProgram({ Opcode::PLP, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePLPTest, ReadsFromWhereTheStackPointerIsPointing)
	{
		cpu.StackPointer = 0x7F;
		memory[StackAddress(0x80)] = UnrelatedFlags;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
		EXPECT_EQ(cpu.StackPointer, 0x80);
	}

	TEST_F(OpcodePLPTest, WrapsTheStackPointerRoundWhenTheStackIsEmpty)
	{
		cpu.StackPointer = InitialStackPointer;
		memory[StackAddress(0x00)] = UnrelatedFlags;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
		EXPECT_EQ(cpu.StackPointer, 0x00);
	}

	TEST_F(OpcodePLPTest, LeavesTheByteBehindOnTheStack)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = UnrelatedFlags;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], UnrelatedFlags);
	}

	TEST_F(OpcodePLPTest, ReplacesEveryFlagRatherThanMergingThem)
	{
		cpu.ProcessorStatus = 0xFF;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x00;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, 0x00);
	}

	TEST_F(OpcodePLPTest, SetsTheFlagsThatArePulled)
	{
		cpu.ProcessorStatus = 0x00;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = Flag::Carry | Flag::Overflow;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Carry));
		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Decimal));
	}

	TEST_F(OpcodePLPTest, ClearsTheFlagsThatAreNotPulled)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = Flag::Carry;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Carry));
		EXPECT_FALSE(cpu.GetFlag(Flag::InterruptDisable));
		EXPECT_FALSE(cpu.GetFlag(Flag::Decimal));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
	}

	TEST_F(OpcodePLPTest, SetsTheZeroFlagWhenThePulledByteHasItSet)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = Flag::Zero;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodePLPTest, ClearsTheZeroFlagWhenThePulledByteIsZero)
	{
		cpu.ProcessorStatus = Flag::Zero;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = 0x00;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodePLPTest, LeavesTheRegistersAlone)
	{
		cpu.Accumulator = 0x11;
		cpu.XRegister = 0x22;
		cpu.YRegister = 0x33;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 1);
		memory[StackAddress(InitialStackPointer)] = UnrelatedFlags;
		WriteProgram({ Opcode::PLP });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.XRegister, 0x22);
		EXPECT_EQ(cpu.YRegister, 0x33);
	}

	TEST_F(OpcodePLPTest, RestoresTheFlagsSavedByPhp)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::PHP;
		memory[0x4001] = Opcode::LDA_IMMEDIATE;
		memory[0x4002] = 0x00;
		memory[0x4003] = Opcode::PLP;

		cpu.Execute(3 + 3 + 2 + 4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

}
