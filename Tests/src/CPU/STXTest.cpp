#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeSTXZeroPageTest : public CPUTest {};
	class OpcodeSTXZeroPageYTest : public CPUTest {};
	class OpcodeSTXAbsoluteTest : public CPUTest {};

	TEST_F(OpcodeSTXZeroPageTest, StoresTheXRegisterAtTheZeroPageAddress)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTXZeroPageTest, OverwritesTheValueAlreadyHeldAtTheAddress)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37 });
		memory[0x0037] = 0x99;

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
	}

	TEST_F(OpcodeSTXZeroPageTest, ConsumesThreeCycles)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTXZeroPageTest, LeavesTheXRegisterAlone)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
	}

	TEST_F(OpcodeSTXZeroPageTest, StoresZeroWithoutSettingTheZeroFlag)
	{
		cpu.XRegister = 0x00;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37 });
		memory[0x0037] = 0x99;

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x00);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeSTXZeroPageTest, StoresANegativeValueWithoutSettingTheNegativeFlag)
	{
		cpu.XRegister = 0x80;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x80);
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeSTXZeroPageTest, LeavesTheOtherFlagsAlone)
	{
		cpu.XRegister = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeSTXZeroPageTest, LeavesTheOtherRegistersAlone)
	{
		cpu.XRegister = 0x42;
		cpu.Accumulator = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::STX_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeSTXZeroPageYTest, AddsTheYRegisterToTheZeroPageAddress)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::STX_ZERO_PAGE_Y, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x003C], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTXZeroPageYTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::STX_ZERO_PAGE_Y, 0x80 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x007F], 0x42);
		EXPECT_EQ(memory[0x017F], 0x00);
	}

	TEST_F(OpcodeSTXZeroPageYTest, ConsumesFourCycles)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::STX_ZERO_PAGE_Y, 0x37, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x003C], 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTXZeroPageYTest, LeavesTheYRegisterAlone)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::STX_ZERO_PAGE_Y, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x05);
	}

	TEST_F(OpcodeSTXZeroPageYTest, LeavesTheFlagsAlone)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0x05;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::STX_ZERO_PAGE_Y, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeSTXAbsoluteTest, StoresTheXRegisterAtTheAbsoluteAddress)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::STX_ABSOLUTE, 0x80, 0x44 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTXAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::STX_ABSOLUTE, 0x34, 0x12 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x1234], 0x42);
		EXPECT_EQ(memory[0x3412], 0x00);
	}

	TEST_F(OpcodeSTXAbsoluteTest, ConsumesFourCycles)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::STX_ABSOLUTE, 0x80, 0x44, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTXAbsoluteTest, LeavesTheFlagsAlone)
	{
		cpu.XRegister = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::STX_ABSOLUTE, 0x80, 0x44 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

}
