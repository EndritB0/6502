#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeLDXImmediateTest : public CPUTest {};
	class OpcodeLDXZeroPageTest : public CPUTest {};
	class OpcodeLDXZeroPageYTest : public CPUTest {};
	class OpcodeLDXAbsoluteTest : public CPUTest {};
	class OpcodeLDXAbsoluteYTest : public CPUTest {};

	TEST_F(OpcodeLDXImmediateTest, LoadsTheOperandIntoTheXRegister)
	{
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDXImmediateTest, ConsumesTwoCycles)
	{
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x42, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDXImmediateTest, SetsTheZeroFlagWhenTheOperandIsZero)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x00 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDXImmediateTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x80 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDXImmediateTest, ClearsZeroAndNegativeForAPositiveOperand)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDXImmediateTest, LeavesTheOtherFlagsAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeLDXImmediateTest, LeavesTheAccumulatorAndTheYRegisterAlone)
	{
		cpu.Accumulator = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::LDX_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeLDXZeroPageTest, LoadsTheValueHeldAtTheZeroPageAddress)
	{
		WriteProgram({ Opcode::LDX_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDXZeroPageTest, ConsumesThreeCycles)
	{
		WriteProgram({ Opcode::LDX_ZERO_PAGE, 0x42, Opcode::LDX_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDXZeroPageTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::LDX_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x00;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDXZeroPageTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDX_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xF0;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0xF0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDXZeroPageYTest, AddsTheYRegisterToTheZeroPageAddress)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ZERO_PAGE_Y, 0x42 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDXZeroPageYTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::LDX_ZERO_PAGE_Y, 0x80 });
		memory[0x007F] = 0x37;
		memory[0x017F] = 0x99;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
	}

	TEST_F(OpcodeLDXZeroPageYTest, ConsumesFourCycles)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ZERO_PAGE_Y, 0x42, Opcode::LDX_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDXZeroPageYTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ZERO_PAGE_Y, 0x42 });
		memory[0x0047] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDXZeroPageYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ZERO_PAGE_Y, 0x42 });
		memory[0x0047] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDXAbsoluteTest, LoadsTheValueHeldAtTheAbsoluteAddress)
	{
		WriteProgram({ Opcode::LDX_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDXAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		WriteProgram({ Opcode::LDX_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x37;
		memory[0x3412] = 0x99;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
	}

	TEST_F(OpcodeLDXAbsoluteTest, ConsumesFourCycles)
	{
		WriteProgram({ Opcode::LDX_ABSOLUTE, 0x80, 0x44, Opcode::LDX_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDXAbsoluteTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::LDX_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDXAbsoluteTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDX_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDXAbsoluteYTest, AddsTheYRegisterToTheAbsoluteAddress)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDXAbsoluteYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::LDX_ABSOLUTE_Y, 0x02, 0x44 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
	}

	TEST_F(OpcodeLDXAbsoluteYTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ABSOLUTE_Y, 0x80, 0x44, Opcode::LDX_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDXAbsoluteYTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::LDX_ABSOLUTE_Y, 0x02, 0x44, Opcode::LDX_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.XRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDXAbsoluteYTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.XRegister = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDXAbsoluteYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDX_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

}
