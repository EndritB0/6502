#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class M6502IncrementDecrementTests : public testing::Test {
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

		// LoadProgram takes the memory first and returns void, so the reference
		// suite's LoadPrg( Prg, NumBytes, Mem ) -> Word becomes a call with the
		// arguments reordered plus a read of the program's own little endian
		// header for the start address.
		Word LoadPrg(Byte* program, std::uint32_t programSize)
		{
			cpu.LoadProgram(mem, program, programSize);
			return static_cast<Word>(program[0] | (program[1] << 8));
		}

		void ExpectUnaffectedFlags(const CPU& CPUBefore)
		{
			EXPECT_EQ(CPUBefore.GetFlag(Flag::Carry), cpu.GetFlag(Flag::Carry));
			EXPECT_EQ(CPUBefore.GetFlag(Flag::InterruptDisable), cpu.GetFlag(Flag::InterruptDisable));
			EXPECT_EQ(CPUBefore.GetFlag(Flag::Decimal), cpu.GetFlag(Flag::Decimal));
			EXPECT_EQ(CPUBefore.GetFlag(Flag::Break), cpu.GetFlag(Flag::Break));
			EXPECT_EQ(CPUBefore.GetFlag(Flag::Overflow), cpu.GetFlag(Flag::Overflow));
		}
	};

	TEST_F(M6502IncrementDecrementTests, INXCanIncrementAZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0x0;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::INX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0x1);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INXCanIncrement255)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0xFF;
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::INX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0x0);	//NOTE: does this instruction actually wrap?
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INXCanIncrementANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0b10001000;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::INX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0b10001001);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}


	TEST_F(M6502IncrementDecrementTests, INYCanIncrementAZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0x0;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::INY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0x1);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INYCanIncrement255)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0xFF;
		cpu.SetFlag(Flag::Zero, false);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::INY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0x0);	//NOTE: does this instruction actually wrap?
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INYCanIncrementANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0b10001000;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::INY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0b10001001);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}


	TEST_F(M6502IncrementDecrementTests, DEYCanDecementAZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0x0;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::DEY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0xFF);	//NOTE: Does this wrap?
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DEYCanDecrement255)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0xFF;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::DEY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0xFE);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DEYCanDecrementANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.YRegister = 0b10001001;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::DEY;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.YRegister, 0b10001000);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DEXCanDecementAZeroValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0x0;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::DEX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0xFF);	//NOTE: Does this wrap?
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DEXCanDecrement255)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0xFF;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::DEX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0xFE);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DEXCanDecrementANegativeValue)
	{
		// given:
		ResetTo(0xFF00);
		cpu.XRegister = 0b10001001;
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, false);
		mem[0xFF00] = Opcode::DEX;
		constexpr Cycles EXPECTED_CYCLES = 2;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(cpu.XRegister, 0b10001000);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DECCanDecrementAValueInTheZeroPage)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::DEC_ZERO_PAGE;
		mem[0xFF01] = 0x42;
		mem[0x0042] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x0042], 0x56);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DECCanDecrementAValueInTheZeroPageX)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 0x10;
		mem[0xFF00] = Opcode::DEC_ZERO_PAGE_X;
		mem[0xFF01] = 0x42;
		mem[0x0042 + 0x10] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x0042 + 0x10], 0x56);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DECCanDecrementAValueAbsolute)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::DEC_ABSOLUTE;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x8000], 0x56);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, DECCanDecrementAValueAbsoluteX)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 0x10;
		mem[0xFF00] = Opcode::DEC_ABSOLUTE_X;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000 + 0x10] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 7;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x8000 + 0x10], 0x56);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INCCanIncrementAValueInTheZeroPage)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::INC_ZERO_PAGE;
		mem[0xFF01] = 0x42;
		mem[0x0042] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x0042], 0x58);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INCCanIncrementAValueInTheZeroPageX)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 0x10;
		mem[0xFF00] = Opcode::INC_ZERO_PAGE_X;
		mem[0xFF01] = 0x42;
		mem[0x0042 + 0x10] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x0042 + 0x10], 0x58);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INCCanIncrementAValueAbsolute)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFF00] = Opcode::INC_ABSOLUTE;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x8000], 0x58);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, INCCanIncrementAValueAbsoluteX)
	{
		// given:
		ResetTo(0xFF00);
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 0x10;
		mem[0xFF00] = Opcode::INC_ABSOLUTE_X;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000 + 0x10] = 0x57;
		constexpr Cycles EXPECTED_CYCLES = 7;
		CPU CPUCopy = cpu;

		// when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		// then:
		EXPECT_EQ(mem[0x8000 + 0x10], 0x58);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		ExpectUnaffectedFlags(CPUCopy);
	}

	TEST_F(M6502IncrementDecrementTests, TestLoadAProgramThatCanIncMemory)
	{
		// given:

		// when:
		/*
		* = $1000

		lda #00
		sta $42

		start
		inc $42
		ldx $42
		inx
		jmp start
		*/
		Byte TestPrg[] =
		{ 0x0,0x10,0xA9,0x00,0x85,0x42,0xE6,0x42,
		0xA6,0x42,0xE8,0x4C,0x04,0x10 };

		Word StartAddress = LoadPrg(TestPrg, static_cast<std::uint32_t>(sizeof(TestPrg)));
		cpu.ProgramCounter = StartAddress;

		//then:
		// Execute returns void here and already runs until its cycle budget is
		// spent, so the reference suite's clock loop becomes a single call.
		cpu.Execute(1000, mem);
	}

}

#endif
