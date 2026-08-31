#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeEORImmediateTest : public CPUTest {};
	class OpcodeEORZeroPageTest : public CPUTest {};
	class OpcodeEORZeroPageXTest : public CPUTest {};
	class OpcodeEORAbsoluteTest : public CPUTest {};
	class OpcodeEORAbsoluteXTest : public CPUTest {};
	class OpcodeEORAbsoluteYTest : public CPUTest {};
	class OpcodeEORIndirectXTest : public CPUTest {};
	class OpcodeEORIndirectYTest : public CPUTest {};

	TEST_F(OpcodeEORImmediateTest, ExclusiveOrsTheAccumulatorWithTheOperand)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x8A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORImmediateTest, ConsumesTwoCycles)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x8A, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORImmediateTest, FlipsEveryBitWhenTheOperandIsAllOnes)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0xFF });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
	}

	TEST_F(OpcodeEORImmediateTest, KeepsTheAccumulatorWhenTheOperandIsZero)
	{
		cpu.Accumulator = 0x46;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x00 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORImmediateTest, SetsTheZeroFlagWhenTheOperandMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0xCC });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORImmediateTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x0A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeEORImmediateTest, ClearsTheNegativeFlagWhenBothBitSevensAreSet)
	{
		cpu.Accumulator = 0xCC;
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x8A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORImmediateTest, ClearsZeroAndNegativeForAPositiveResult)
	{
		cpu.Accumulator = 0xCC;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x8A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORImmediateTest, LeavesTheOtherFlagsAlone)
	{
		cpu.Accumulator = 0xCC;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x8A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeEORImmediateTest, LeavesTheIndexRegistersAlone)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::EOR_IMMEDIATE, 0x8A });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeEORZeroPageTest, ExclusiveOrsTheAccumulatorWithTheValueHeldAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x8A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORZeroPageTest, ConsumesThreeCycles)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x8A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORZeroPageTest, SetsTheZeroFlagWhenTheValueMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xCC;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORZeroPageTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x0A;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeEORZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::EOR_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x8A;
		memory[0x017F] = 0xFF;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORZeroPageXTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ZERO_PAGE_X, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORZeroPageXTest, SetsTheZeroFlagWhenTheValueMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0xCC;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORZeroPageXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeEORAbsoluteTest, ExclusiveOrsTheAccumulatorWithTheValueHeldAtTheAbsoluteAddress)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x8A;
		memory[0x3412] = 0xFF;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORAbsoluteTest, ConsumesFourCycles)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteTest, SetsTheZeroFlagWhenTheValueMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0xCC;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORAbsoluteTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeEORAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::EOR_ABSOLUTE_X, 0x02, 0x44 });
		memory[0x4501] = 0x8A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORAbsoluteXTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_X, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteXTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::EOR_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x8A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteXTest, SetsTheZeroFlagWhenTheValueMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0xCC;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORAbsoluteXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeEORAbsoluteYTest, AddsTheYRegisterToTheAbsoluteAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::EOR_ABSOLUTE_Y, 0x02, 0x44 });
		memory[0x4501] = 0x8A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORAbsoluteYTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_Y, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x8A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteYTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::EOR_ABSOLUTE_Y, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x8A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeEORAbsoluteYTest, SetsTheZeroFlagWhenTheValueMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0xCC;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORAbsoluteYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::EOR_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x0A;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeEORIndirectXTest, ReadsThePointerHeldAtTheZeroPageAddressOffsetByX)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x8A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORIndirectXTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x34;
		memory[0x0007] = 0x12;
		memory[0x1234] = 0x8A;
		memory[0x3412] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORIndirectXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x03;
		WriteProgram({ Opcode::EOR_INDIRECT_X, 0xFE });
		memory[0x0001] = 0x00;
		memory[0x0002] = 0x80;
		memory[0x8000] = 0x8A;
		memory[0x0101] = 0x00;
		memory[0x0102] = 0x90;
		memory[0x9000] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORIndirectXTest, WrapsThePointerFetchInsideTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_X, 0xFB });
		memory[0x00FF] = 0x00;
		memory[0x0000] = 0x80;
		memory[0x8000] = 0x8A;
		memory[0x0100] = 0x90;
		memory[0x9000] = 0xFF;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORIndirectXTest, ConsumesSixCycles)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_X, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x8A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORIndirectXTest, SetsTheZeroFlagWhenTheValueMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0xCC;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORIndirectXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x0A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeEORIndirectYTest, AddsTheYRegisterToThePointerHeldAtTheZeroPageAddress)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x8A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORIndirectYTest, ReadsThePointerLittleEndian)
	{
		cpu.Accumulator = 0xCC;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x34;
		memory[0x0003] = 0x12;
		memory[0x1234] = 0x8A;
		memory[0x3412] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORIndirectYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x8A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORIndirectYTest, WrapsThePointerFetchInsideTheZeroPage)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0xFF });
		memory[0x00FF] = 0x00;
		memory[0x0000] = 0x80;
		memory[0x8004] = 0x8A;
		memory[0x0100] = 0x90;
		memory[0x9004] = 0xFF;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
	}

	TEST_F(OpcodeEORIndirectYTest, ConsumesFiveCyclesWhenNoPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x8A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORIndirectYTest, ConsumesSixCyclesWhenAPageIsCrossed)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x8A;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x46);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeEORIndirectYTest, SetsTheZeroFlagWhenTheValueMatchesTheAccumulator)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0xCC;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeEORIndirectYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::EOR_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x0A;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0xC6);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

}
