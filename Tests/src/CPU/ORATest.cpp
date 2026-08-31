#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeORAImmediateTest : public CPUTest {};
	class OpcodeORAZeroPageTest : public CPUTest {};
	class OpcodeORAZeroPageXTest : public CPUTest {};
	class OpcodeORAAbsoluteTest : public CPUTest {};
	class OpcodeORAAbsoluteXTest : public CPUTest {};
	class OpcodeORAAbsoluteYTest : public CPUTest {};
	class OpcodeORAIndirectXTest : public CPUTest {};
	class OpcodeORAIndirectYTest : public CPUTest {};

	TEST_F(OpcodeORAImmediateTest, OrsTheAccumulatorWithTheOperand)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x0A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAImmediateTest, ConsumesTwoCycles)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x0A, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAImmediateTest, SetsEveryBitWhenTheOperandIsAllOnes)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0xFF });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0xFF);
	}

	TEST_F(OpcodeORAImmediateTest, KeepsTheAccumulatorWhenTheOperandIsZero)
	{
		cpu.Accumulator = 0x4E;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x00 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAImmediateTest, SetsTheZeroFlagWhenTheAccumulatorAndTheOperandAreBothZero)
	{
		cpu.Accumulator = 0x00;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x00 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAImmediateTest, LeavesTheZeroFlagClearWhenAnyBitIsSet)
	{
		cpu.Accumulator = 0x00;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x01 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x01);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAImmediateTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x8A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAImmediateTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.Accumulator = 0x4C;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x0A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAImmediateTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0x4C;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x0A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeORAImmediateTest, LeavesTheIndexRegistersAlone)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::ORA_IMMEDIATE, 0x0A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeORAZeroPageTest, OrsTheAccumulatorWithTheValueHeldAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x0A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAZeroPageTest, ConsumesThreeCycles)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x0A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAZeroPageTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueAreBothZero)
	{
		cpu.Accumulator = 0x00;
		WriteProgram({ Opcode::ORA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x00;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAZeroPageTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x8A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::ORA_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x0A;
		memory[0x017F] = 0xFF;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAZeroPageXTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ZERO_PAGE_X, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAZeroPageXTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueAreBothZero)
	{
		cpu.Accumulator = 0x00;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAZeroPageXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAAbsoluteTest, OrsTheAccumulatorWithTheValueHeldAtTheAbsoluteAddress)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x0A;
		memory[0x3412] = 0xFF;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAAbsoluteTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueAreBothZero)
	{
		cpu.Accumulator = 0x00;
		WriteProgram({ Opcode::ORA_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAAbsoluteTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::ORA_ABSOLUTE_X, 0x02, 0x44 });
		memory[0x4501] = 0x0A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAAbsoluteXTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_X, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteXTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::ORA_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x0A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteXTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueAreBothZero)
	{
		cpu.Accumulator = 0x00;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAAbsoluteXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAAbsoluteYTest, AddsTheYRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::ORA_ABSOLUTE_Y, 0x02, 0x44 });
		memory[0x4501] = 0x0A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAAbsoluteYTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_Y, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteYTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::ORA_ABSOLUTE_Y, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x0A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeORAAbsoluteYTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueAreBothZero)
	{
		cpu.Accumulator = 0x00;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAAbsoluteYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::ORA_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAIndirectXTest, ReadsThePointerHeldAtTheZeroPageAddressOffsetByX)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x0A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAIndirectXTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x34;
		memory[0x0007] = 0x12;
		memory[0x1234] = 0x0A;
		memory[0x3412] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAIndirectXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x03;
		WriteProgram({ Opcode::ORA_INDIRECT_X, 0xFE });
		memory[0x0001] = 0x00;
		memory[0x0002] = 0x80;
		memory[0x8000] = 0x0A;
		memory[0x0101] = 0x00;
		memory[0x0102] = 0x90;
		memory[0x9000] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAIndirectXTest, WrapsThePointerFetchInsideTheZeroPage)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_X, 0xFB });
		memory[0x00FF] = 0x00;
		memory[0x0000] = 0x80;
		memory[0x8000] = 0x0A;
		memory[0x0100] = 0x90;
		memory[0x9000] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAIndirectXTest, ConsumesSixCycles)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_X, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x0A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAIndirectXTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueAreBothZero)
	{
		cpu.Accumulator = 0x00;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x00;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAIndirectXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x8A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeORAIndirectYTest, AddsTheYRegisterToThePointerHeldAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x0A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAIndirectYTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0x4C;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x34;
		memory[0x0003] = 0x12;
		memory[0x1234] = 0x0A;
		memory[0x3412] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAIndirectYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x0A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAIndirectYTest, WrapsThePointerFetchInsideTheZeroPage)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0xFF });
		memory[0x00FF] = 0x00;
		memory[0x0000] = 0x80;
		memory[0x8004] = 0x0A;
		memory[0x0100] = 0x90;
		memory[0x9004] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
	}

	TEST_F(OpcodeORAIndirectYTest, ConsumesFiveCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x0A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAIndirectYTest, ConsumesSixCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x0A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x4E);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeORAIndirectYTest, SetsTheZeroFlagWhenTheAccumulatorAndTheValueAreBothZero)
	{
		cpu.Accumulator = 0x00;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x00;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeORAIndirectYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0x4C;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::ORA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x8A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0xCE);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

}
