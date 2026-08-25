#include "pch.h"

#include <CPU.h>

namespace Test6502 {

	using MOS6502::CPU;
	using MOS6502::Memory;

	using MOS6502::Address;
	using MOS6502::Byte;
	using MOS6502::Cycles;
	using MOS6502::Word;

	namespace Flag = MOS6502::Flag;
	namespace Opcode = MOS6502::Opcode;

	constexpr Address ResetVector{ 0xFFFC };
	constexpr Word InitialStackPointer{ 0x0100 };
	constexpr Byte UnrelatedFlags{ Flag::Carry | Flag::InterruptDisable | Flag::Decimal | Flag::Overflow };

	class CPUTest : public ::testing::Test {
	protected:
		void SetUp() override
		{
			cpu.Reset(memory);
		}

		void WriteProgram(std::initializer_list<Byte> bytes)
		{
			Address address{ ResetVector };
			for (const Byte byte : bytes)
			{
				memory[address] = byte;
				address++;
			}
		}

		CPU cpu;
		Memory memory;
	};

	class OpcodeLDAImmediateTest : public CPUTest {};
	class OpcodeLDAZeroPageTest : public CPUTest {};
	class OpcodeLDAZeroPageXTest : public CPUTest {};
	class OpcodeLDAAbsoluteTest : public CPUTest {};
	class OpcodeLDAAbsoluteXTest : public CPUTest {};
	class OpcodeLDAAbsoluteYTest : public CPUTest {};
	class OpcodeLDAIndirectXTest : public CPUTest {};
	class OpcodeLDAIndirectYTest : public CPUTest {};
	class OpcodeLDXImmediateTest : public CPUTest {};
	class OpcodeLDXZeroPageTest : public CPUTest {};
	class OpcodeLDXZeroPageYTest : public CPUTest {};
	class OpcodeLDXAbsoluteTest : public CPUTest {};
	class OpcodeLDXAbsoluteYTest : public CPUTest {};
	class OpcodeJSRTest : public CPUTest {};

	TEST_F(OpcodeLDAImmediateTest, LoadsTheOperandIntoTheAccumulator)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAImmediateTest, ConsumesTwoCycles)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAImmediateTest, SetsTheZeroFlagWhenTheOperandIsZero)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x00 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAImmediateTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x80 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAImmediateTest, ClearsZeroAndNegativeForAPositiveOperand)
	{
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAImmediateTest, LeavesTheOtherFlagsAlone)
	{
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeLDAImmediateTest, LeavesTheIndexRegistersAlone)
	{
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::LDA_IMMEDIATE, 0x42 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeLDAZeroPageTest, LoadsTheValueHeldAtTheZeroPageAddress)
	{
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageTest, ConsumesThreeCycles)
	{
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0042] = 0x37;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0x00;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAZeroPageTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDA_ZERO_PAGE, 0x42 });
		memory[0x0042] = 0xF0;

		cpu.Execute(3, memory);

		EXPECT_EQ(cpu.Accumulator, 0xF0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAZeroPageXTest, AddsTheXRegisterToTheZeroPageAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x80 });
		memory[0x007F] = 0x37;
		memory[0x017F] = 0x99;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAZeroPageXTest, ConsumesFourCycles)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0047] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAZeroPageXTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAZeroPageXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ZERO_PAGE_X, 0x42 });
		memory[0x0047] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAAbsoluteTest, LoadsTheValueHeldAtTheAbsoluteAddress)
	{
		WriteProgram({ Opcode::LDA_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteTest, ReadsTheAddressOperandLittleEndian)
	{
		WriteProgram({ Opcode::LDA_ABSOLUTE, 0x34, 0x12 });
		memory[0x1234] = 0x37;
		memory[0x3412] = 0x99;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAAbsoluteTest, ConsumesFourCycles)
	{
		WriteProgram({ Opcode::LDA_ABSOLUTE, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4480] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		WriteProgram({ Opcode::LDA_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAAbsoluteTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		WriteProgram({ Opcode::LDA_ABSOLUTE, 0x80, 0x44 });
		memory[0x4480] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAAbsoluteXTest, AddsTheXRegisterToTheAbsoluteAddress)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteXTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::LDA_ABSOLUTE_X, 0x02, 0x44 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAAbsoluteXTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_X, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteXTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.XRegister = 0xFF;
		WriteProgram({ Opcode::LDA_ABSOLUTE_X, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteXTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAAbsoluteXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_X, 0x80, 0x44 });
		memory[0x4485] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAAbsoluteYTest, AddsTheYRegisterToTheAbsoluteAddress)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::LDA_ABSOLUTE_Y, 0x02, 0x44 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAAbsoluteYTest, ConsumesFourCyclesWhenNoPageIsCrossed)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_Y, 0x80, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4485] = 0x37;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteYTest, ConsumesFiveCyclesWhenAPageIsCrossed)
	{
		cpu.YRegister = 0xFF;
		WriteProgram({ Opcode::LDA_ABSOLUTE_Y, 0x02, 0x44, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x4501] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 3);
	}

	TEST_F(OpcodeLDAAbsoluteYTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x00;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAAbsoluteYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.YRegister = 0x05;
		WriteProgram({ Opcode::LDA_ABSOLUTE_Y, 0x80, 0x44 });
		memory[0x4485] = 0x80;

		cpu.Execute(4, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAIndirectXTest, ReadsThePointerHeldAtTheZeroPageAddressOffsetByX)
	{
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x37;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAIndirectXTest, ReadsThePointerLittleEndian)
	{
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x34;
		memory[0x0007] = 0x12;
		memory[0x1234] = 0x37;
		memory[0x3412] = 0x99;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAIndirectXTest, WrapsTheSumBackIntoTheZeroPage)
	{
		cpu.XRegister = 0x03;
		WriteProgram({ Opcode::LDA_INDIRECT_X, 0xFE });
		memory[0x0001] = 0x00;
		memory[0x0002] = 0x80;
		memory[0x8000] = 0x37;
		memory[0x0101] = 0x00;
		memory[0x0102] = 0x90;
		memory[0x9000] = 0x99;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAIndirectXTest, ConsumesSixCycles)
	{
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_X, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x37;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAIndirectXTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x00;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAIndirectXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.XRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_X, 0x02 });
		memory[0x0006] = 0x00;
		memory[0x0007] = 0x80;
		memory[0x8000] = 0x80;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeLDAIndirectYTest, AddsTheYRegisterToThePointerHeldAtTheZeroPageAddress)
	{
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAIndirectYTest, ReadsThePointerLittleEndian)
	{
		WriteProgram({ Opcode::LDA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x34;
		memory[0x0003] = 0x12;
		memory[0x1234] = 0x37;
		memory[0x3412] = 0x99;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAIndirectYTest, CarriesTheSumIntoTheNextPage)
	{
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::LDA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x37;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
	}

	TEST_F(OpcodeLDAIndirectYTest, ConsumesFiveCyclesWhenNoPageIsCrossed)
	{
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x37;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAIndirectYTest, ConsumesSixCyclesWhenAPageIsCrossed)
	{
		cpu.YRegister = 0x01;
		WriteProgram({ Opcode::LDA_INDIRECT_Y, 0x02, Opcode::LDA_IMMEDIATE, 0x99 });
		memory[0x0002] = 0xFF;
		memory[0x0003] = 0x80;
		memory[0x8100] = 0x37;

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 2);
	}

	TEST_F(OpcodeLDAIndirectYTest, SetsTheZeroFlagWhenTheLoadedValueIsZero)
	{
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x00;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeLDAIndirectYTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.YRegister = 0x04;
		WriteProgram({ Opcode::LDA_INDIRECT_Y, 0x02 });
		memory[0x0002] = 0x00;
		memory[0x0003] = 0x80;
		memory[0x8004] = 0x80;

		cpu.Execute(5, memory);

		EXPECT_EQ(cpu.Accumulator, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

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

	TEST_F(OpcodeJSRTest, JumpsToTheSubroutineAddress)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.ProgramCounter, 0x4242);
	}

	TEST_F(OpcodeJSRTest, SavesTheAddressOfTheLastInstructionByteLittleEndian)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(memory[InitialStackPointer], 0xFE);
		EXPECT_EQ(memory[InitialStackPointer + 1], 0xFF);
	}

	TEST_F(OpcodeJSRTest, MovesTheStackPointerPastTheSavedAddress)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.StackPointer, InitialStackPointer + 2);
	}

	TEST_F(OpcodeJSRTest, LeavesTheRegistersAndFlagsAlone)
	{
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x11;
		cpu.YRegister = 0x22;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });

		cpu.Execute(6, memory);

		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeJSRTest, ConsumesSixCyclesAndCarriesOnIntoTheSubroutine)
	{
		WriteProgram({ Opcode::JSR, 0x42, 0x42 });
		memory[0x4242] = Opcode::LDA_IMMEDIATE;
		memory[0x4243] = 0x37;

		cpu.Execute(6 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x4244);
	}

}
