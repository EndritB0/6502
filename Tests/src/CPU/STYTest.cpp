#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeSTYZeroPageTest : public CPUTest {};
	class OpcodeSTYZeroPageXTest : public CPUTest {};
	class OpcodeSTYAbsoluteTest : public CPUTest {};

	TEST_F(OpcodeSTYZeroPageTest, StoresTheYRegisterAtTheZeroPageAddress)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTYZeroPageTest, OverwritesTheValueAlreadyHeldAtTheAddress)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37 });
		memory[0x0037] = 0x99;

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
	}

	TEST_F(OpcodeSTYZeroPageTest, ConsumesThreeCycles)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37, Opcode::LDY_IMMEDIATE, 0x99 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTYZeroPageTest, LeavesTheYRegisterAlone)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
	}

	TEST_F(OpcodeSTYZeroPageTest, StoresZeroWithoutSettingTheZeroFlag)
	{
		cpu.YRegister = 0x00;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37 });
		memory[0x0037] = 0x99;

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x00);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeSTYZeroPageTest, StoresANegativeValueWithoutSettingTheNegativeFlag)
	{
		cpu.YRegister = 0x80;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x80);
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeSTYZeroPageTest, LeavesTheOtherFlagsAlone)
	{
		cpu.YRegister = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeSTYZeroPageTest, LeavesTheOtherRegistersAlone)
	{
		cpu.YRegister = 0x42;
		cpu.Accumulator = 0x11;
		cpu.XRegister = 0x22;
		WriteProgram({ Opcode::STY_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.XRegister, 0x22);
	}

	TEST_F(OpcodeSTYZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.YRegister = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STY_ZERO_PAGE_X, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x003C], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTYZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.YRegister = 0x42;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::STY_ZERO_PAGE_X, 0x80 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x007F], 0x42);
		EXPECT_EQ(memory[0x017F], 0x00);
	}

	TEST_F(OpcodeSTYZeroPageXTest, ConsumesFourCycles)
	{
		cpu.YRegister = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STY_ZERO_PAGE_X, 0x37, Opcode::LDY_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x003C], 0x42);
		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTYZeroPageXTest, LeavesTheXRegisterAlone)
	{
		cpu.YRegister = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STY_ZERO_PAGE_X, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x05);
	}

	TEST_F(OpcodeSTYZeroPageXTest, LeavesTheFlagsAlone)
	{
		cpu.YRegister = 0x42;
		cpu.XRegister = 0x05;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::STY_ZERO_PAGE_X, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeSTYAbsoluteTest, StoresTheYRegisterAtTheAbsoluteAddress)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::STY_ABSOLUTE, 0x80, 0x44 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTYAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::STY_ABSOLUTE, 0x34, 0x12 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x1234], 0x42);
		EXPECT_EQ(memory[0x3412], 0x00);
	}

	TEST_F(OpcodeSTYAbsoluteTest, ConsumesFourCycles)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::STY_ABSOLUTE, 0x80, 0x44, Opcode::LDY_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTYAbsoluteTest, LeavesTheFlagsAlone)
	{
		cpu.YRegister = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::STY_ABSOLUTE, 0x80, 0x44 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

}
