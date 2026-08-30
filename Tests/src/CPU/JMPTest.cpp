#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeJMPAbsoluteTest : public CPUTest {};
	class OpcodeJMPIndirectTest : public CPUTest {};

	TEST_F(OpcodeJMPAbsoluteTest, JumpsToTheAbsoluteAddress)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x42, 0x42 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4242);
	}

	TEST_F(OpcodeJMPAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x34, 0x12 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x1234);
	}

	TEST_F(OpcodeJMPAbsoluteTest, JumpsToAnAddressInTheZeroPage)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x80, 0x00 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x0080);
	}

	TEST_F(OpcodeJMPAbsoluteTest, ConsumesThreeCyclesAndCarriesOnAtTheTarget)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x42, 0x42 });
		memory[0x4242] = Opcode::LDA_IMMEDIATE;
		memory[0x4243] = 0x37;

		cpu.Execute(3 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4244);
	}

	TEST_F(OpcodeJMPAbsoluteTest, LeavesTheStackAlone)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x42, 0x42 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeJMPAbsoluteTest, LeavesTheRegistersAndFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x42, 0x42 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeJMPIndirectTest, JumpsToTheAddressHeldAtThePointer)
	{
		WriteProgram({ Opcode::JMP_INDIRECT, 0x40, 0x30 });
		memory[0x3040] = 0x42;
		memory[0x3041] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4342);
	}

	TEST_F(OpcodeJMPIndirectTest, ReadsThePointerOperandLittleEndian)
	{
		WriteProgram({ Opcode::JMP_INDIRECT, 0x34, 0x12 });
		memory[0x1234] = 0x80;
		memory[0x1235] = 0x44;
		memory[0x3412] = 0x99;
		memory[0x3413] = 0x99;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4480);
	}

	TEST_F(OpcodeJMPIndirectTest, WrapsTheHighByteInsideThePageWhenThePointerEndsAtFF)
	{
		WriteProgram({ Opcode::JMP_INDIRECT, 0xFF, 0x30 });
		memory[0x30FF] = 0x40;
		memory[0x3000] = 0x80;
		memory[0x3100] = 0x99;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x8040);
	}

	TEST_F(OpcodeJMPIndirectTest, ConsumesFiveCyclesAndCarriesOnAtTheTarget)
	{
		WriteProgram({ Opcode::JMP_INDIRECT, 0x40, 0x30 });
		memory[0x3040] = 0x42;
		memory[0x3041] = 0x43;
		memory[0x4342] = Opcode::LDA_IMMEDIATE;
		memory[0x4343] = 0x37;

		cpu.Execute(5 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4344);
	}

	TEST_F(OpcodeJMPIndirectTest, LeavesTheStackAlone)
	{
		WriteProgram({ Opcode::JMP_INDIRECT, 0x40, 0x30 });
		memory[0x3040] = 0x42;
		memory[0x3041] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
		EXPECT_EQ(memory[StackAddress(0xFF)], 0x00);
		EXPECT_EQ(memory[StackAddress(0xFE)], 0x00);
	}

	TEST_F(OpcodeJMPIndirectTest, LeavesTheRegistersAndFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::JMP_INDIRECT, 0x40, 0x30 });
		memory[0x3040] = 0x42;
		memory[0x3041] = 0x43;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

}
