#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodePHATest : public CPUTest {};

	TEST_F(OpcodePHATest, PushesTheAccumulatorOntoTheStack)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePHATest, MovesTheStackPointerDownOneByte)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer - 1);
	}

	TEST_F(OpcodePHATest, ConsumesThreeCycles)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::PHA, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodePHATest, LeavesTheAccumulatorAlone)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodePHATest, WritesWhereTheStackPointerIsPointing)
	{
		cpu.Accumulator = 0x42;
		cpu.StackPointer = 0x80;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(0x80)], 0x42);
		EXPECT_EQ(cpu.StackPointer, 0x7F);
	}

	TEST_F(OpcodePHATest, WrapsTheStackPointerRoundWhenTheStackIsFull)
	{
		cpu.Accumulator = 0x42;
		cpu.StackPointer = 0x00;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(0x00)], 0x42);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodePHATest, StaysInsideTheStackPage)
	{
		cpu.Accumulator = 0x42;
		cpu.StackPointer = 0x80;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0080], 0x00);
		EXPECT_EQ(memory[0x0280], 0x00);
	}

	TEST_F(OpcodePHATest, OverwritesTheByteAlreadyOnTheStack)
	{
		cpu.Accumulator = 0x42;
		memory[StackAddress(InitialStackPointer)] = 0x99;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], 0x42);
	}

	TEST_F(OpcodePHATest, PushesZeroWithoutSettingTheZeroFlag)
	{
		cpu.Accumulator = 0x00;
		memory[StackAddress(InitialStackPointer)] = 0x99;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], 0x00);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodePHATest, PushesANegativeValueWithoutSettingTheNegativeFlag)
	{
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[StackAddress(InitialStackPointer)], 0x80);
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodePHATest, LeavesTheFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodePHATest, LeavesTheOtherRegistersAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::PHA });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodePHATest, StacksSuccessivePushesDownwards)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDA_IMMEDIATE;
		memory[0x4001] = 0x11;
		memory[0x4002] = Opcode::PHA;
		memory[0x4003] = Opcode::LDA_IMMEDIATE;
		memory[0x4004] = 0x22;
		memory[0x4005] = Opcode::PHA;

		cpu.Execute(3 + 2 + 3 + 2 + 3, memory);

		EXPECT_EQ(memory[StackAddress(0xFF)], 0x11);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x22);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer - 2);
	}

	TEST_F(OpcodePHATest, PushesBelowAReturnAddressAlreadyOnTheStack)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::JSR, 0x00, 0x40 });
		memory[0x4000] = Opcode::PHA;

		cpu.Execute(6 + 3, memory);

		EXPECT_EQ(memory[StackAddress(0xFF)], 0xFF);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0xFE);
		EXPECT_EQ(memory[StackAddress(0xFD)], 0x42);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer - 3);
	}

}
