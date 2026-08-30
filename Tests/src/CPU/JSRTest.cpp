#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeJSRTest : public CPUTest {};

	TEST_F(OpcodeJSRTest, JumpsToTheSubroutineAddress)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4242);
	}

	TEST_F(OpcodeJSRTest, ReadsTheSubroutineAddressLittleEndian)
	{
		WriteProgram({ Opcode::JSR, 0x34, 0x12 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x1234);
	}

	TEST_F(OpcodeJSRTest, SavesTheAddressOfTheLastInstructionByteLittleEndian)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[StackAddress(0xFE)], 0xFE);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0xFF);
	}

	TEST_F(OpcodeJSRTest, MovesTheStackPointerDownPastTheSavedAddress)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer - 2);
	}

	TEST_F(OpcodeJSRTest, PushesToWhereTheStackPointerIsPointing)
	{
		cpu.StackPointer = 0x80;
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[StackAddress(0x7F)], 0xFE);
		EXPECT_EQ(memory[StackAddress(0x80)], 0xFF);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(cpu.StackPointer, 0x7E);
	}

	TEST_F(OpcodeJSRTest, LeavesTheRegistersAndFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeJSRTest, ConsumesSixCyclesAndCarriesOnIntoTheSubroutine)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });
		memory[0x4242] = Opcode::LDA_IMMEDIATE;
		memory[0x4243] = 0x37;

		cpu.Execute(6 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4244);
	}

}
