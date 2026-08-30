#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class M6502StackOperationsTests : public testing::Test {
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

	TEST_F(M6502StackOperationsTests, TSXCanTransferTheStackPointerToXRegister)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 0x00;
		cpu.StackPointer = 0x01;
		mem[0xFF00] = Opcode::TSX;
		constexpr Cycles EXPECTED_CYCLES = 2;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0x01);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(M6502StackOperationsTests, TSXCanTransferAZeroStackPointerToXRegister)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 0x00;
		cpu.StackPointer = 0x00;
		mem[0xFF00] = Opcode::TSX;
		constexpr Cycles EXPECTED_CYCLES = 2;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(M6502StackOperationsTests, TSXCanTransferANegativeStackPointerToXRegister)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, false);
		cpu.XRegister = 0x00;
		cpu.StackPointer = 0b10000000;
		mem[0xFF00] = Opcode::TSX;
		constexpr Cycles EXPECTED_CYCLES = 2;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0b10000000);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
	}

	TEST_F(M6502StackOperationsTests, TXSCanTransferXRegisterToTheStackPointer)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0xFF;
		cpu.StackPointer = 0;
		mem[0xFF00] = Opcode::TXS;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.StackPointer, 0xFF);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502StackOperationsTests, PHACanPushARegsiterOntoTheStack)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0x42;
		mem[0xFF00] = Opcode::PHA;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[static_cast<Address>(cpu.GetStackAddress() + 1)], cpu.Accumulator);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
		EXPECT_EQ(cpu.StackPointer, 0xFE);
	}

	TEST_F(M6502StackOperationsTests, PLACanPullAValueFromTheStackIntoTheARegsiter)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0x00;
		cpu.StackPointer = 0xFE;
		mem[0x01FF] = 0x42;
		mem[0xFF00] = Opcode::PLA;
		constexpr Cycles EXPECTED_CYCLES = 4;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.StackPointer, 0xFF);
	}

	TEST_F(M6502StackOperationsTests, PLACanPullAZeroValueFromTheStackIntoTheARegsiter)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, true);
		cpu.Accumulator = 0x42;
		cpu.StackPointer = 0xFE;
		mem[0x01FF] = 0x00;
		mem[0xFF00] = Opcode::PLA;
		constexpr Cycles EXPECTED_CYCLES = 4;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0x00);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_EQ(cpu.StackPointer, 0xFF);
	}

	TEST_F(M6502StackOperationsTests, PLACanPullANegativeValueFromTheStackIntoTheARegsiter)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Negative, false);
		cpu.SetFlag(Flag::Zero, true);
		cpu.Accumulator = 0x42;
		cpu.StackPointer = 0xFE;
		mem[0x01FF] = 0b10000001;
		mem[0xFF00] = Opcode::PLA;
		constexpr Cycles EXPECTED_CYCLES = 4;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0b10000001);
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.StackPointer, 0xFF);
	}

	TEST_F(M6502StackOperationsTests, PHPCanPushProcessorStatusOntoTheStack)
	{
		// given:
		ResetTo(0xFF00);
		cpu.ProcessorStatus = 0xCC;
		mem[0xFF00] = Opcode::PHP;
		constexpr Cycles EXPECTED_CYCLES = 3;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then: this core pushes the status with the break and unused bits set.
		EXPECT_EQ(mem[static_cast<Address>(cpu.GetStackAddress() + 1)], 0xCC | Flag::Break | Flag::Unused);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
		EXPECT_EQ(cpu.StackPointer, 0xFE);
	}

	TEST_F(M6502StackOperationsTests, PLPCanPullAValueFromTheStackIntoTheProcessorStatus)
	{
		// given:
		ResetTo(0xFF00);
		cpu.StackPointer = 0xFE;
		cpu.ProcessorStatus = 0;
		mem[0x01FF] = 0x42;
		mem[0xFF00] = Opcode::PLP;
		constexpr Cycles EXPECTED_CYCLES = 4;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.ProcessorStatus, 0x42);
	}

}

#endif
