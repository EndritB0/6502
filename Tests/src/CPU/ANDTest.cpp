#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeANDImmediateTest : public CPUTest {};
	class OpcodeANDZeroPageTest : public CPUTest {};
	class OpcodeANDZeroPageXTest : public CPUTest {};
	class OpcodeANDAbsoluteTest : public CPUTest {};
	class OpcodeANDAbsoluteXTest : public CPUTest {};
	class OpcodeANDAbsoluteYTest : public CPUTest {};
	class OpcodeANDIndirectXTest : public CPUTest {};
	class OpcodeANDIndirectYTest : public CPUTest {};

	TEST_F(OpcodeANDImmediateTest, MasksTheAccumulatorWithTheOperand)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_IMMEDIATE, 0x7A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDImmediateTest, ConsumesTwoCycles)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_IMMEDIATE, 0x7A, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDImmediateTest, KeepsTheAccumulatorWhenTheOperandIsAllOnes)
	{
		cpu.Accumulator = 0x48;
		WriteProgram({ Opcode::AND_IMMEDIATE, 0xFF });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDImmediateTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		WriteProgram({ Opcode::AND_IMMEDIATE, 0x0F });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDImmediateTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_IMMEDIATE, 0xAA });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeANDImmediateTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.Accumulator = 0xCC;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::AND_IMMEDIATE, 0x7A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDImmediateTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0xCC;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::AND_IMMEDIATE, 0x7A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeANDImmediateTest, LeavesTheIndexRegistersAlone)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::AND_IMMEDIATE, 0x7A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeANDZeroPageTest, MasksTheAccumulatorWithTheValueHeldAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x7A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDZeroPageTest, ConsumesThreeCycles)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x7A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDZeroPageTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		WriteProgram({ Opcode::AND_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x0F;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDZeroPageTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xAA;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeANDZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::AND_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x7A;
		memory[0x017F] = 0xFF;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDZeroPageXTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ZERO_PAGE_X, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDZeroPageXTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x0F;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDZeroPageXTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0xAA;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeANDAbsoluteTest, MasksTheAccumulatorWithTheValueHeldAtTheAbsoluteAddress)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x7A;
		memory[0x3412] = 0xFF;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDAbsoluteTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		WriteProgram({ Opcode::AND_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x0F;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDAbsoluteTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0xAA;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeANDAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::AND_ABSOLUTE_X, 0x02, 0x44 });
		memory[0x4501] = 0x7A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDAbsoluteXTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_X, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteXTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::AND_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x7A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteXTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x0F;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDAbsoluteXTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0xAA;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeANDAbsoluteYTest, AddsTheYRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::AND_ABSOLUTE_Y, 0x02, 0x44 });
		memory[0x4501] = 0x7A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDAbsoluteYTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_Y, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x7A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteYTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::AND_ABSOLUTE_Y, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x7A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeANDAbsoluteYTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x0F;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDAbsoluteYTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::AND_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0xAA;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeANDIndirectXTest, ReadsThePointerHeldAtTheZeroPageAddressOffsetByX)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x7A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDIndirectXTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x34;
		memory[0x0007] = 0x12;
		memory[0x1234] = 0x7A;
		memory[0x3412] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDIndirectXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x03;
		WriteProgram({ Opcode::AND_INDIRECT_X, 0xFE });
		memory[0x0001] = 0x00;
		memory[0x0002] = 0x80;
		memory[0x8000] = 0x7A;
		memory[0x0101] = 0x00;
		memory[0x0102] = 0x90;
		memory[0x9000] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDIndirectXTest, WrapsThePointerFetchInsideTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_X, 0xFB });
		memory[0x00FF] = 0x00;
		memory[0x0000] = 0x80;
		memory[0x8000] = 0x7A;
		memory[0x0100] = 0x90;
		memory[0x9000] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDIndirectXTest, ConsumesSixCycles)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_X, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x7A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDIndirectXTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x0F;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDIndirectXTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0xAA;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeANDIndirectYTest, AddsTheYRegisterToThePointerHeldAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x7A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDIndirectYTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x34;
		memory[0x0003] = 0x12;
		memory[0x1234] = 0x7A;
		memory[0x3412] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDIndirectYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x7A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDIndirectYTest, WrapsThePointerFetchInsideTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0xFF });
		memory[0x00FF] = 0x00;
		memory[0x0000] = 0x80;
		memory[0x8004] = 0x7A;
		memory[0x0100] = 0x90;
		memory[0x9004] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
	}

	TEST_F(OpcodeANDIndirectYTest, ConsumesFiveCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x7A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDIndirectYTest, ConsumesSixCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x7A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x48);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeANDIndirectYTest, SetsTheZeroFlagWhenNoBitsSurvive)
	{
		cpu.Accumulator = 0xF0;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x0F;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeANDIndirectYTest, SetsTheNegativeFlagWhenBitSevenSurvives)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::AND_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0xAA;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x88);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

}
