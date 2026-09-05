#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class M6502BranchTests : public testing::Test {
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

	TEST_F(M6502BranchTests, BEQCanBranchForwardsWhenZeroIsSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		mem[0xFF00] = Opcode::BEQ;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BEQDoesNotBranchForwardsWhenZeroIsNotSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, false);
		mem[0xFF00] = Opcode::BEQ;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF02);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BEQCanBranchForwardsIntoANewPageWhenZeroIsSet)
	{
		// given:
		ResetTo(0xFEFD);
		cpu.SetFlag(Flag::Zero, true);
		mem[0xFEFD] = Opcode::BEQ;
		mem[0xFEFE] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF00);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BEQCanBranchBackwardsWhenZeroIsSet)
	{
		// given:
		ResetTo(0xFFCC);
		cpu.SetFlag(Flag::Zero, true);
		mem[0xFFCC] = Opcode::BEQ;
		mem[0xFFCD] = static_cast<Byte>(-0x2);
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFFCC);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BEQCanBranchBackwardsWhenZeroIsSetFromAssembleCode)
	{
		// given:
		ResetTo(0xFFCC);
		cpu.SetFlag(Flag::Zero, true);
		/*
		loop
		lda #0
		beq loop
		*/
		mem[0xFFCC] = 0xA9;
		mem[0xFFCC + 1] = 0x00;
		mem[0xFFCC + 2] = 0xF0;
		mem[0xFFCC + 3] = 0xFC;
		constexpr Cycles EXPECTED_CYCLES = 2 + 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFFCC);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BNECanBranchForwardsWhenZeroIsNotSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, false);
		mem[0xFF00] = Opcode::BNE;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BCSCanBranchForwardsWhenCarryFlagIsSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Carry, true);
		mem[0xFF00] = Opcode::BCS;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BCCCanBranchForwardsWhenCarryFlagIsNotSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Carry, false);
		mem[0xFF00] = Opcode::BCC;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BMICanBranchForwardsWhenNegativeFlagIsSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::BMI;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BPLCanBranchForwardsWhenCarryNegativeIsNotSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::BPL;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BVSCanBranchForwardsWhenOverflowFlagIsSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Overflow, true);
		mem[0xFF00] = Opcode::BVS;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502BranchTests, BVCCanBranchForwardsWhenOverflowNegativeIsNotSet)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Overflow, false);
		mem[0xFF00] = Opcode::BVC;
		mem[0xFF01] = 0x1;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProgramCounter, 0xFF03);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

}

#endif
