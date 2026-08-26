#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeLDYImmediateTest : public CPUTest {};
	class OpcodeLDYZeroPageTest : public CPUTest {};
	class OpcodeLDYZeroPageXTest : public CPUTest {};
	class OpcodeLDYAbsoluteTest : public CPUTest {};
	class OpcodeLDYAbsoluteXTest : public CPUTest {};

	TEST_F(OpcodeLDYImmediateTest, LoadsTheOperandIntoTheYRegister)
	{
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDYImmediateTest, ConsumesTwoCycles)
	{
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x42, Opcode::LDY_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDYImmediateTest, SetsTheZeroFlagWhenTheOperandIsZero)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x00 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDYImmediateTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x80 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.YRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDYImmediateTest, ClearsZeroAndNegativeForAPositiveOperand)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDYImmediateTest, LeavesTheOtherFlagsAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeLDYImmediateTest, LeavesTheAccumulatorAndTheXRegisterAlone)
	{
		cpu.Accumulator = 0x11;
		cpu.XRegister = 0x22;
		WriteProgram({ Opcode::LDY_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.XRegister, 0x22);
	}

	TEST_F(OpcodeLDYZeroPageTest, LoadsTheValueHeldAtTheZeroPageAddress)
	{
		WriteProgram({ Opcode::LDY_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDYZeroPageTest, ConsumesThreeCycles)
	{
		WriteProgram({ Opcode::LDY_ZERO_PAGE, 0x42, Opcode::LDY_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDYZeroPageTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::LDY_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x00;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDYZeroPageTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDY_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xF0;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.YRegister, 0xF0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDYZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDY_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDYZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::LDY_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x37;
		memory[0x017F] = 0x99;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
	}

	TEST_F(OpcodeLDYZeroPageXTest, ConsumesFourCycles)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDY_ZERO_PAGE_X, 0x42, Opcode::LDY_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDYZeroPageXTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.XRegister = 0x05;
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::LDY_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDYZeroPageXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDY_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDYAbsoluteTest, LoadsTheValueHeldAtTheAbsoluteAddress)
	{
		WriteProgram({ Opcode::LDY_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDYAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		WriteProgram({ Opcode::LDY_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x37;
		memory[0x3412] = 0x99;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
	}

	TEST_F(OpcodeLDYAbsoluteTest, ConsumesFourCycles)
	{
		WriteProgram({ Opcode::LDY_ABSOLUTE, 0x80, 0x44, Opcode::LDY_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDYAbsoluteTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::LDY_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDYAbsoluteTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDY_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDYAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDY_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDYAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::LDY_ABSOLUTE_X, 0x02, 0x44 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
	}

	TEST_F(OpcodeLDYAbsoluteXTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDY_ABSOLUTE_X, 0x80, 0x44, Opcode::LDY_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDYAbsoluteXTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::LDY_ABSOLUTE_X, 0x02, 0x44, Opcode::LDY_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.YRegister, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDYAbsoluteXTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.XRegister = 0x05;
		cpu.YRegister = 0x42;
		WriteProgram({ Opcode::LDY_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDYAbsoluteXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDY_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.YRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

}
