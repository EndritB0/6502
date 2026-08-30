#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeTXSTest : public CPUTest {};

	TEST_F(OpcodeTXSTest, CopiesTheXRegisterIntoTheStackPointer)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.StackPointer, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTXSTest, ConsumesTwoCycles)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXS, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.StackPointer, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTXSTest, OverwritesTheStackPointerLeftByTheReset)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.StackPointer, 0x42);
		EXPECT_NE(cpu.StackPointer, InitialStackPointer);
	}

	TEST_F(OpcodeTXSTest, LeavesTheXRegisterAlone)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
	}

	TEST_F(OpcodeTXSTest, CopiesZeroWithoutSettingTheZeroFlag)
	{
		cpu.XRegister = 0x00;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.StackPointer, 0x00);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeTXSTest, CopiesANegativeValueWithoutSettingTheNegativeFlag)
	{
		cpu.XRegister = 0x80;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.StackPointer, 0x80);
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTXSTest, LeavesTheZeroAndNegativeFlagsAsItFoundThem)
	{
		cpu.XRegister = 0x42;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTXSTest, LeavesTheOtherFlagsAlone)
	{
		cpu.XRegister = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus, UnrelatedFlags);
	}

	TEST_F(OpcodeTXSTest, LeavesTheAccumulatorAndTheYRegisterAlone)
	{
		cpu.XRegister = 0x42;
		cpu.Accumulator = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeTXSTest, LeavesTheStackContentsAlone)
	{
		cpu.XRegister = 0x42;
		memory[StackAddress(0x42)] = 0x37;
		memory[StackAddress(0x43)] = 0x38;
		WriteProgram({ Opcode::TXS });

		cpu.Execute(2, memory);

		EXPECT_EQ(memory[StackAddress(0x42)], 0x37);
		EXPECT_EQ(memory[StackAddress(0x43)], 0x38);
	}

	TEST_F(OpcodeTXSTest, PointsTheStackAtWhereTheNextPushLands)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::LDX_IMMEDIATE;
		memory[0x4001] = 0x80;
		memory[0x4002] = Opcode::TXS;
		memory[0x4003] = Opcode::JSR;
		memory[0x4004] = 0x00;
		memory[0x4005] = 0x41;

		cpu.Execute(3 + 2 + 2 + 6, memory);

		EXPECT_EQ(memory[StackAddress(0x7F)], 0x05);
		EXPECT_EQ(memory[StackAddress(0x80)], 0x40);
		EXPECT_EQ(cpu.StackPointer, 0x7E);
		EXPECT_EQ(cpu.ProgramCounter, 0x4100);
	}

	TEST_F(OpcodeTXSTest, HandsTheStackPointerBackToTheXRegisterThroughTsx)
	{
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TXS, Opcode::LDX_IMMEDIATE, 0x99, Opcode::TSX });

		cpu.Execute(2 + 2 + 2, memory);

		EXPECT_EQ(cpu.StackPointer, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
	}

}
