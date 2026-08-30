#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeRTSTest : public CPUTest {};

	TEST_F(OpcodeRTSTest, JumpsPastTheAddressSavedOnTheStack)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 2);
		memory[StackAddress(0xFE)] = 0x41;
		memory[StackAddress(0xFF)] = 0x42;
		WriteProgram({ Opcode::RTS });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4242);
	}

	TEST_F(OpcodeRTSTest, ReadsTheSavedAddressLittleEndian)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 2);
		memory[StackAddress(0xFE)] = 0x34;
		memory[StackAddress(0xFF)] = 0x12;
		WriteProgram({ Opcode::RTS });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x1235);
	}

	TEST_F(OpcodeRTSTest, MovesTheStackPointerBackUpPastTheSavedAddress)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 2);
		memory[StackAddress(0xFE)] = 0x41;
		memory[StackAddress(0xFF)] = 0x42;
		WriteProgram({ Opcode::RTS });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeRTSTest, PullsFromWhereTheStackPointerIsPointing)
	{
		cpu.StackPointer = 0x7E;
		memory[StackAddress(0x7F)] = 0x41;
		memory[StackAddress(0x80)] = 0x42;
		WriteProgram({ Opcode::RTS });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4242);
		EXPECT_EQ(cpu.StackPointer, 0x80);
	}

	TEST_F(OpcodeRTSTest, ConsumesSixCycles)
	{
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 2);
		memory[StackAddress(0xFE)] = 0x41;
		memory[StackAddress(0xFF)] = 0x42;
		WriteProgram({ Opcode::RTS });
		memory[0x4242] = Opcode::LDA_IMMEDIATE;
		memory[0x4243] = 0x37;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_EQ(cpu.ProgramCounter, 0x4242);
	}

	TEST_F(OpcodeRTSTest, LeavesTheRegistersAndFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.ProcessorStatus = UnrelatedFlags;
		cpu.StackPointer = static_cast<Byte>(InitialStackPointer - 2);
		memory[StackAddress(0xFE)] = 0x41;
		memory[StackAddress(0xFF)] = 0x42;
		WriteProgram({ Opcode::RTS });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeRTSTest, ReturnsToTheInstructionAfterTheJumpToSubroutine)
	{
		WriteProgram({ Opcode::JSR, 0x00, 0x40 });
		memory[0x4000] = Opcode::RTS;

		cpu.Execute(6 + 6, memory);

		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeRTSTest, RunsTheSubroutineAndCarriesOnAfterTheReturn)
	{
		WriteProgram({ Opcode::JSR, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDA_IMMEDIATE;
		memory[0x4001] = 0x37;
		memory[0x4002] = Opcode::RTS;

		cpu.Execute(6 + 2 + 6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeRTSTest, UnwindsNestedSubroutinesInOrder)
	{
		WriteProgram({ Opcode::JSR, 0x00, 0x40 });
		memory[0x4000] = Opcode::JSR;
		memory[0x4001] = 0x00;
		memory[0x4002] = 0x41;
		memory[0x4003] = Opcode::RTS;
		memory[0x4100] = Opcode::RTS;

		cpu.Execute(6 + 6 + 6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4003);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer - 2);

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

}
