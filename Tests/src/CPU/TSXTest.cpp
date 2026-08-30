#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class OpcodeTSXTest : public CPUTest {};

	TEST_F(OpcodeTSXTest, CopiesTheStackPointerIntoTheXRegister)
	{
		cpu.StackPointer = 0x42;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTSXTest, ConsumesTwoCycles)
	{
		cpu.StackPointer = 0x42;
		WriteProgram({ Opcode::TSX, Opcode::LDX_IMMEDIATE, 0x99 });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.ProgramCounter, ResetVector + 1);
	}

	TEST_F(OpcodeTSXTest, OverwritesTheValueAlreadyInTheXRegister)
	{
		cpu.StackPointer = 0x42;
		cpu.XRegister = 0x99;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x42);
	}

	TEST_F(OpcodeTSXTest, ReadsBackTheStackPointerLeftByTheReset)
	{
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, InitialStackPointer);
	}

	TEST_F(OpcodeTSXTest, SetsTheZeroFlagWhenTheStackPointerIsZero)
	{
		cpu.StackPointer = 0x00;
		cpu.XRegister = 0x42;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTSXTest, SetsTheNegativeFlagWhenBitSevenIsSet)
	{
		cpu.StackPointer = 0x80;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.XRegister, 0x80);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(OpcodeTSXTest, ClearsZeroAndNegativeForAPositiveStackPointer)
	{
		cpu.StackPointer = 0x42;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(OpcodeTSXTest, LeavesTheOtherFlagsAlone)
	{
		cpu.StackPointer = 0x42;
		cpu.ProcessorStatus = UnrelatedFlags;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.ProcessorStatus & UnrelatedFlags, UnrelatedFlags);
	}

	TEST_F(OpcodeTSXTest, LeavesTheStackPointerAlone)
	{
		cpu.StackPointer = 0x42;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.StackPointer, 0x42);
	}

	TEST_F(OpcodeTSXTest, LeavesTheAccumulatorAndTheYRegisterAlone)
	{
		cpu.StackPointer = 0x42;
		cpu.Accumulator = 0x11;
		cpu.YRegister = 0x22;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x11);
		EXPECT_EQ(cpu.YRegister, 0x22);
	}

	TEST_F(OpcodeTSXTest, LeavesTheStackContentsAlone)
	{
		cpu.StackPointer = 0x42;
		memory[StackAddress(0x42)] = 0x37;
		memory[StackAddress(0x43)] = 0x38;
		WriteProgram({ Opcode::TSX });

		cpu.Execute(2, memory);

		EXPECT_EQ(memory[StackAddress(0x42)], 0x37);
		EXPECT_EQ(memory[StackAddress(0x43)], 0x38);
	}

	TEST_F(OpcodeTSXTest, SeesTheStackPointerPushedDownByAJumpToSubroutine)
	{
		WriteProgram({ Opcode::JSR, 0x00, 0x40 });
		memory[0x4000] = Opcode::TSX;

		cpu.Execute(6 + 2, memory);

		EXPECT_EQ(cpu.XRegister, InitialStackPointer - 2);
	}

	TEST_F(OpcodeTSXTest, SeesTheStackPointerBackUpAgainAfterTheReturn)
	{
		WriteProgram({ Opcode::JMP_ABSOLUTE, 0x00, 0x40 });
		memory[0x4000] = Opcode::JSR;
		memory[0x4001] = 0x00;
		memory[0x4002] = 0x41;
		memory[0x4003] = Opcode::TSX;
		memory[0x4100] = Opcode::RTS;

		cpu.Execute(3 + 6 + 6 + 2, memory);

		EXPECT_EQ(cpu.XRegister, InitialStackPointer);
		EXPECT_EQ(cpu.ProgramCounter, 0x4004);
	}

}
