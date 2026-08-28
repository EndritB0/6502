#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	static void VerfifyUnmodifiedFlagsFromStoreRegister(
		const CPU& cpu,
		const CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.GetFlag(Flag::Carry), CPUCopy.GetFlag(Flag::Carry));
		EXPECT_EQ(cpu.GetFlag(Flag::InterruptDisable), CPUCopy.GetFlag(Flag::InterruptDisable));
		EXPECT_EQ(cpu.GetFlag(Flag::Decimal), CPUCopy.GetFlag(Flag::Decimal));
		EXPECT_EQ(cpu.GetFlag(Flag::Break), CPUCopy.GetFlag(Flag::Break));
		EXPECT_EQ(cpu.GetFlag(Flag::Overflow), CPUCopy.GetFlag(Flag::Overflow));
		EXPECT_EQ(cpu.GetFlag(Flag::Zero), CPUCopy.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), CPUCopy.GetFlag(Flag::Negative));
	}

	class M6502StoreRegisterTests : public testing::Test {
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

		void TestStoreRegisterZeroPage(
			Byte OpcodeToTest,
			Byte CPU::* Register)
		{
			// given:
			cpu.*Register = 0x2F;
			mem[0xFFFC] = OpcodeToTest;
			mem[0xFFFD] = 0x80;
			mem[0x0080] = 0x00;
			constexpr Cycles EXPECTED_CYCLES = 3;
			CPU CPUCopy = cpu;

			// when:
			cpu.Execute(EXPECTED_CYCLES, mem);

			// then:
			EXPECT_EQ(mem[0x0080], 0x2F);
			VerfifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
		}

		void TestStoreRegisterAbsolute(
			Byte OpcodeToTest,
			Byte CPU::* Register)
		{
			// given:
			cpu.*Register = 0x2F;
			mem[0xFFFC] = OpcodeToTest;
			mem[0xFFFD] = 0x00;
			mem[0xFFFE] = 0x80;
			mem[0x8000] = 0x00;
			constexpr Cycles EXPECTED_CYCLES = 4;
			CPU CPUCopy = cpu;

			// when:
			cpu.Execute(EXPECTED_CYCLES, mem);

			// then:
			EXPECT_EQ(mem[0x8000], 0x2F);
			VerfifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
		}

		void TestStoreRegisterZeroPageX(
			Byte OpcodeToTest,
			Byte CPU::* Register)
		{
			// given:
			cpu.*Register = 0x42;
			cpu.XRegister = 0x0F;
			mem[0xFFFC] = OpcodeToTest;
			mem[0xFFFD] = 0x80;
			mem[0x008F] = 0x00;
			constexpr Cycles EXPECTED_CYCLES = 4;
			CPU CPUCopy = cpu;

			// when:
			cpu.Execute(EXPECTED_CYCLES, mem);

			// then:
			EXPECT_EQ(mem[0x008F], 0x42);
			VerfifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
		}
	};

	TEST_F(M6502StoreRegisterTests, STAZeroPageCanStoreTheARegisterIntoMemory)
	{
		TestStoreRegisterZeroPage(Opcode::STA_ZERO_PAGE, &CPU::Accumulator);
	}

	TEST_F(M6502StoreRegisterTests, STXZeroPageCanStoreTheXRegisterIntoMemory)
	{
		TestStoreRegisterZeroPage(Opcode::STX_ZERO_PAGE, &CPU::XRegister);
	}

	TEST_F(M6502StoreRegisterTests, STYZeroPageCanStoreTheYRegisterIntoMemory)
	{
		TestStoreRegisterZeroPage(Opcode::STY_ZERO_PAGE, &CPU::YRegister);
	}

	TEST_F(M6502StoreRegisterTests, STAAbsoluteCanStoreTheARegisterIntoMemory)
	{
		TestStoreRegisterAbsolute(Opcode::STA_ABSOLUTE, &CPU::Accumulator);
	}

	TEST_F(M6502StoreRegisterTests, STXAbsoluteCanStoreTheXRegisterIntoMemory)
	{
		TestStoreRegisterAbsolute(Opcode::STX_ABSOLUTE, &CPU::XRegister);
	}

	TEST_F(M6502StoreRegisterTests, STYAbsoluteCanStoreTheYRegisterIntoMemory)
	{
		TestStoreRegisterAbsolute(Opcode::STY_ABSOLUTE, &CPU::YRegister);
	}

	TEST_F(M6502StoreRegisterTests, STAZeroPageXCanStoreTheARegisterIntoMemory)
	{
		TestStoreRegisterZeroPageX(Opcode::STA_ZERO_PAGE_X, &CPU::Accumulator);
	}

	TEST_F(M6502StoreRegisterTests, STYZeroPageXCanStoreTheARegisterIntoMemory)
	{
		TestStoreRegisterZeroPageX(Opcode::STY_ZERO_PAGE_X, &CPU::YRegister);
	}

	TEST_F(M6502StoreRegisterTests, STAAbsoluteXCanStoreTheARegisterIntoMemory)
	{
		// given:
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x0F;
		mem[0xFFFC] = Opcode::STA_ABSOLUTE_X;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x80;
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x800F], 0x42);
		VerfifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
	}

	TEST_F(M6502StoreRegisterTests, STAAbsoluteYCanStoreTheARegisterIntoMemory)
	{
		// given:
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x0F;
		mem[0xFFFC] = Opcode::STA_ABSOLUTE_Y;
		mem[0xFFFD] = 0x00;
		mem[0xFFFE] = 0x80;
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x800F], 0x42);
		VerfifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
	}

	TEST_F(M6502StoreRegisterTests, STAIndirectXCanStoreTheARegisterIntoMemory)
	{
		// given:
		cpu.Accumulator = 0x42;
		cpu.XRegister = 0x0F;
		mem[0xFFFC] = Opcode::STA_INDIRECT_X;
		mem[0xFFFD] = 0x20;
		mem[0x002F] = 0x00;
		mem[0x0030] = 0x80;
		mem[0x8000] = 0x00;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x8000], 0x42);
		VerfifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
	}

	TEST_F(M6502StoreRegisterTests, STAIndirectYCanStoreTheARegisterIntoMemory)
	{
		// given:
		cpu.Accumulator = 0x42;
		cpu.YRegister = 0x0F;
		mem[0xFFFC] = Opcode::STA_INDIRECT_Y;
		mem[0xFFFD] = 0x20;
		mem[0x0020] = 0x00;
		mem[0x0021] = 0x80;
		mem[0x8000 + 0x0F] = 0x00;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x8000 + 0x0F], 0x42);
		VerfifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
	}

}

#endif
