#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	constexpr Byte BITUntouchedFlags{ Flag::Carry | Flag::InterruptDisable | Flag::Decimal };

	class OpcodeBITZeroPageTest : public CPUTest {};
	class OpcodeBITAbsoluteTest : public CPUTest {};

	TEST_F(OpcodeBITZeroPageTest, ClearsTheZeroFlagWhenTheAccumulatorAndTheValueShareABit)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x18;

		cpu.Execute(3, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeBITZeroPageTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueShareNoBits)
	{
		cpu.Accumulator = 0xF0;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x0F;

		cpu.Execute(3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeBITZeroPageTest, ConsumesThreeCycles)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x18;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x3C);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeBITZeroPageTest, LeavesTheAccumulatorAlone)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x18;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x3C);
	}

	TEST_F(OpcodeBITZeroPageTest, LeavesTheTestedValueAlone)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x18;

		cpu.Execute(3, memory);

		EXPECT_EQ(memory[0x0042], 0x18);
	}

	TEST_F(OpcodeBITZeroPageTest, CopiesBitSevenOfTheValueIntoTheNegativeFlag)
	{
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x80;

		cpu.Execute(3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITZeroPageTest, CopiesBitSixOfTheValueIntoTheOverflowFlag)
	{
		cpu.Accumulator = 0x40;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x40;

		cpu.Execute(3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITZeroPageTest, SetsBothTheNegativeAndOverflowFlagsWhenTheTopTwoBitsAreSet)
	{
		cpu.Accumulator = 0xC0;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xC0;

		cpu.Execute(3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITZeroPageTest, TakesTheNegativeAndOverflowFlagsFromTheValueNotTheAccumulator)
	{
		cpu.Accumulator = 0xC0;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x3F;

		cpu.Execute(3, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITZeroPageTest, SetsTheNegativeAndOverflowFlagsEvenWhenTheTestResultIsZero)
	{
		cpu.Accumulator = 0x01;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xC0;

		cpu.Execute(3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
	}

	TEST_F(OpcodeBITZeroPageTest, ClearsZeroNegativeAndOverflowWhenNoneOfThemApply)
	{
		cpu.Accumulator = 0x01;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x3F;

		cpu.Execute(3, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
	}

	TEST_F(OpcodeBITZeroPageTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0x3C;
		cpu.ProcessorStatus = BITUntouchedFlags;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x18;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.ProcessorStatus & BITUntouchedFlags, BITUntouchedFlags);
	}

	TEST_F(OpcodeBITZeroPageTest, LeavesTheIndexRegistersAlone)
	{
		cpu.Accumulator = 0x3C;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x18;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeBITZeroPageTest, ReadsTheValueFromInsideTheZeroPage)
	{
		cpu.Accumulator = 0xFF;
		WriteProgram({ Opcode::BIT_ZERO_PAGE, 0x80 });
		memory[0x0080] = 0x40;
		memory[0x0180] = 0x80;

		cpu.Execute(3, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeBITAbsoluteTest, ClearsTheZeroFlagWhenTheAccumulatorAndTheValueShareABit)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x18;

		cpu.Execute(4, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeBITAbsoluteTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueShareNoBits)
	{
		cpu.Accumulator = 0xF0;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x0F;

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeBITAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		cpu.Accumulator = 0xFF;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x40;
		memory[0x3412] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeBITAbsoluteTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x18;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x3C);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeBITAbsoluteTest, LeavesTheAccumulatorAlone)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x18;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x3C);
	}

	TEST_F(OpcodeBITAbsoluteTest, LeavesTheTestedValueAlone)
	{
		cpu.Accumulator = 0x3C;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x18;

		cpu.Execute(4, memory);

		EXPECT_EQ(memory[0x4480], 0x18);
	}

	TEST_F(OpcodeBITAbsoluteTest, CopiesBitSevenOfTheValueIntoTheNegativeFlag)
	{
		cpu.Accumulator = 0x80;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITAbsoluteTest, CopiesBitSixOfTheValueIntoTheOverflowFlag)
	{
		cpu.Accumulator = 0x40;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x40;

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITAbsoluteTest, SetsBothTheNegativeAndOverflowFlagsWhenTheTopTwoBitsAreSet)
	{
		cpu.Accumulator = 0xC0;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0xC0;

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITAbsoluteTest, TakesTheNegativeAndOverflowFlagsFromTheValueNotTheAccumulator)
	{
		cpu.Accumulator = 0xC0;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x3F;

		cpu.Execute(4, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeBITAbsoluteTest, SetsTheNegativeAndOverflowFlagsEvenWhenTheTestResultIsZero)
	{
		cpu.Accumulator = 0x01;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0xC0;

		cpu.Execute(4, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_TRUE(cpu.GetFlag(Flag::Overflow));
	}

	TEST_F(OpcodeBITAbsoluteTest, ClearsZeroNegativeAndOverflowWhenNoneOfThemApply)
	{
		cpu.Accumulator = 0x01;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.SetFlag(Flag::Overflow, true);
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x3F;

		cpu.Execute(4, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Overflow));
	}

	TEST_F(OpcodeBITAbsoluteTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0x3C;
		cpu.ProcessorStatus = BITUntouchedFlags;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x18;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.ProcessorStatus & BITUntouchedFlags, BITUntouchedFlags);
	}

	TEST_F(OpcodeBITAbsoluteTest, LeavesTheIndexRegistersAlone)
	{
		cpu.Accumulator = 0x3C;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::BIT_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x18;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

}
