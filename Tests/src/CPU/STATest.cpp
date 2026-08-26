#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeSTAZeroPageTest : public CPUTest {};
	class OpcodeSTAZeroPageXTest : public CPUTest {};
	class OpcodeSTAAbsoluteTest : public CPUTest {};
	class OpcodeSTAAbsoluteXTest : public CPUTest {};
	class OpcodeSTAAbsoluteYTest : public CPUTest {};
	class OpcodeSTAIndirectXTest : public CPUTest {};
	class OpcodeSTAIndirectYTest : public CPUTest {};

	TEST_F(OpcodeSTAZeroPageTest, StoresTheAccumulatorAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAZeroPageTest, OverwritesTheValueAlreadyHeldAtTheAddress)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37 });
		memory[0x0037] = 0x99;

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
	}

	TEST_F(OpcodeSTAZeroPageTest, ConsumesThreeCycles)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAZeroPageTest, LeavesTheAccumulatorAlone)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
	}

	TEST_F(OpcodeSTAZeroPageTest, StoresZeroWithoutSettingTheZeroFlag)
	{
		cpu.Accumulator = 0x00;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37 });
		memory[0x0037] = 0x99;

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x00);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeSTAZeroPageTest, StoresANegativeValueWithoutSettingTheNegativeFlag)
	{
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0037], 0x80);
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeSTAZeroPageTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeSTAZeroPageTest, LeavesTheIndexRegistersAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::STA_ZERO_PAGE, 0x37 });

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeSTAZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STA_ZERO_PAGE_X, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x003C], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::STA_ZERO_PAGE_X, 0x80 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x007F], 0x42);
		EXPECT_EQ(memory[0x017F], 0x00);
	}

	TEST_F(OpcodeSTAZeroPageXTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STA_ZERO_PAGE_X, 0x37, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x003C], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAZeroPageXTest, LeavesTheXRegisterAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STA_ZERO_PAGE_X, 0x37 });

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x05);
	}

	TEST_F(OpcodeSTAAbsoluteTest, StoresTheAccumulatorAtTheAbsoluteAddress)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_ABSOLUTE, 0x80, 0x44 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_ABSOLUTE, 0x34, 0x12 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x1234], 0x42);
		EXPECT_EQ(memory[0x3412], 0x00);
	}

	TEST_F(OpcodeSTAAbsoluteTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x4480], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STA_ABSOLUTE_X, 0x80, 0x44 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::STA_ABSOLUTE_X, 0x02, 0x44 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(memory[0x4401], 0x00);
	}

	TEST_F(OpcodeSTAAbsoluteXTest, ConsumesFiveCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::STA_ABSOLUTE_X, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAAbsoluteXTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::STA_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAAbsoluteYTest, AddsTheYRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::STA_ABSOLUTE_Y, 0x80, 0x44 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAAbsoluteYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::STA_ABSOLUTE_Y, 0x02, 0x44 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(memory[0x4401], 0x00);
	}

	TEST_F(OpcodeSTAAbsoluteYTest, ConsumesFiveCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::STA_ABSOLUTE_Y, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4485], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAAbsoluteYTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::STA_ABSOLUTE_Y, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(5, memory);

		EXPECT_EQ(memory[0x4501], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeSTAIndirectXTest, WritesThroughThePointerHeldAtTheZeroPageAddressOffsetByX)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::STA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x8000], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAIndirectXTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::STA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x34;
		memory[0x0007] = 0x12;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x1234], 0x42);
		EXPECT_EQ(memory[0x3412], 0x00);
	}

	TEST_F(OpcodeSTAIndirectXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x03;
		WriteProgram({ Opcode::STA_INDIRECT_X, 0xFE });
		memory[0x0001] = 0x00;
		memory[0x0002] = 0x80;
		memory[0x0101] = 0x00;
		memory[0x0102] = 0x90;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x8000], 0x42);
		EXPECT_EQ(memory[0x9000], 0x00);
	}

	TEST_F(OpcodeSTAIndirectXTest, ConsumesSixCycles)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::STA_INDIRECT_X, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x8000], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAIndirectYTest, AddsTheYRegisterToThePointerHeldAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::STA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x8004], 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAIndirectYTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::STA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x34;
		memory[0x0003] = 0x12;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x1234], 0x42);
		EXPECT_EQ(memory[0x3412], 0x00);
	}

	TEST_F(OpcodeSTAIndirectYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::STA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x8100], 0x42);
		EXPECT_EQ(memory[0x8000], 0x00);
	}

	TEST_F(OpcodeSTAIndirectYTest, ConsumesSixCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::STA_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x8004], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeSTAIndirectYTest, ConsumesSixCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::STA_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[0x8100], 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

}
