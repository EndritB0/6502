#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class M6502JumpsAndCallsTests : public testing::Test {
	public:
		Memory mem;
		CPU cpu;

		virtual void SetUp()
		{
			cpu.Reset(mem);
		}

		virtual void TearDown()
		{
		}

		// Reset always lands on 0xFFFC here, so the reference suite's
		// Reset( Address, Mem ) becomes a reset followed by a jump.
		void ResetTo(Address programCounter)
		{
			cpu.Reset(mem);
			cpu.ProgramCounter = programCounter;
		}
	};

	TEST_F(M6502JumpsAndCallsTests, CanJumpToASubroutineAndJumpBackAgain)
	{
		// given:
		ResetTo(0xFF00);
		mem[0xFF00] = Opcode::JSR;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000] = Opcode::RTS;
		mem[0xFF03] = Opcode::LDA_IMMEDIATE;
		mem[0xFF04] = 0x42;
		constexpr Cycles EXPECTED_CYCLES = 6 + 6 + 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.StackPointer, CPUCopy.StackPointer);
	}

	TEST_F(M6502JumpsAndCallsTests, JSRDoesNotAffectTheProcessorStatus)
	{
		// given:
		ResetTo(0xFF00);
		mem[0xFF00] = Opcode::JSR;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
		EXPECT_NE(cpu.StackPointer, CPUCopy.StackPointer);
		EXPECT_EQ(cpu.ProgramCounter, 0x8000);
	}

	TEST_F(M6502JumpsAndCallsTests, RTSDoesNotAffectTheProcessorStatus)
	{
		// given:
		ResetTo(0xFF00);
		mem[0xFF00] = Opcode::JSR;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000] = Opcode::RTS;
		constexpr Cycles EXPECTED_CYCLES = 6 + 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
	}

	TEST_F(M6502JumpsAndCallsTests, JumpAbsoluteCanJumpToAnNewLocationInTheProgram)
	{
		// given:
		ResetTo(0xFF00);
		mem[0xFF00] = Opcode::JMP_ABSOLUTE;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
		EXPECT_EQ(cpu.StackPointer, CPUCopy.StackPointer);
		EXPECT_EQ(cpu.ProgramCounter, 0x8000);
	}

	TEST_F(M6502JumpsAndCallsTests, JumpIndirectCanJumpToAnNewLocationInTheProgram)
	{
		// given:
		ResetTo(0xFF00);
		mem[0xFF00] = Opcode::JMP_INDIRECT;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000] = 0x00;
		mem[0x8001] = 0x90;
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
		EXPECT_EQ(cpu.StackPointer, CPUCopy.StackPointer);
		EXPECT_EQ(cpu.ProgramCounter, 0x9000);
	}

}

#endif
