#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	static void VerfifyUnmodifiedFlagsFromTransferRegister(
		const CPU& cpu,
		const CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.GetFlag(Flag::Carry), CPUCopy.GetFlag(Flag::Carry));
		EXPECT_EQ(cpu.GetFlag(Flag::InterruptDisable), CPUCopy.GetFlag(Flag::InterruptDisable));
		EXPECT_EQ(cpu.GetFlag(Flag::Decimal), CPUCopy.GetFlag(Flag::Decimal));
		EXPECT_EQ(cpu.GetFlag(Flag::Break), CPUCopy.GetFlag(Flag::Break));
		EXPECT_EQ(cpu.GetFlag(Flag::Overflow), CPUCopy.GetFlag(Flag::Overflow));
	}

	class M6502TransferRegistgerTests : public testing::Test {
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

	TEST_F(M6502TransferRegistgerTests, TAXCanTransferANonNegativeNonZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TAX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TAXCanTransferANonNegativeZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0x0;
		cpu.XRegister = 0x32;
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TAX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0x0);
		EXPECT_EQ(cpu.XRegister, 0x0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TAXCanTransferANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0b10001011;
		cpu.XRegister = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::TAX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0b10001011);
		EXPECT_EQ(cpu.XRegister, 0b10001011);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TAYCanTransferANonNegativeNonZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TAY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TAYCanTransferANonNegativeZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0x0;
		cpu.YRegister = 0x32;
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TAY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0x0);
		EXPECT_EQ(cpu.YRegister, 0x0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TAYCanTransferANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.Accumulator = 0b10001011;
		cpu.YRegister = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::TAY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.Accumulator, 0b10001011);
		EXPECT_EQ(cpu.YRegister, 0b10001011);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TXACanTransferANonNegativeNonZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0x42;
		cpu.Accumulator = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TXA;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TXACanTransferANonNegativeZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0x0;
		cpu.Accumulator = 0x32;
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TXA;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0x0);
		EXPECT_EQ(cpu.Accumulator, 0x0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TXACanTransferANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0b10001011;
		cpu.Accumulator = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::TXA;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0b10001011);
		EXPECT_EQ(cpu.Accumulator, 0b10001011);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TYACanTransferANonNegativeNonZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0x42;
		cpu.Accumulator = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TYA;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0x42);
		EXPECT_EQ(cpu.Accumulator, 0x42);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TYACanTransferANonNegativeZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0x0;
		cpu.Accumulator = 0x32;
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::TYA;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0x0);
		EXPECT_EQ(cpu.Accumulator, 0x0);
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

	TEST_F(M6502TransferRegistgerTests, TYACanTransferANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0b10001011;
		cpu.Accumulator = 0x32;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::TYA;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0b10001011);
		EXPECT_EQ(cpu.Accumulator, 0b10001011);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromTransferRegister(cpu, CPUCopy);
	}

}

#endif
