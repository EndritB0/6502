#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	static void VerfifyUnmodifiedFlagsFromLogicalOpInstruction(
		const CPU& cpu,
		const CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.GetFlag(Flag::Carry), CPUCopy.GetFlag(Flag::Carry));
		EXPECT_EQ(cpu.GetFlag(Flag::InterruptDisable), CPUCopy.GetFlag(Flag::InterruptDisable));
		EXPECT_EQ(cpu.GetFlag(Flag::Decimal), CPUCopy.GetFlag(Flag::Decimal));
		EXPECT_EQ(cpu.GetFlag(Flag::Break), CPUCopy.GetFlag(Flag::Break));
		EXPECT_EQ(cpu.GetFlag(Flag::Overflow), CPUCopy.GetFlag(Flag::Overflow));
	}

	class M6502AndEorOraTests : public testing::Test {
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

		enum class ELogicalOp {
			And, Eor, Or
		};

		Byte DoLogicalOp(
			Byte A,
			Byte B,
			ELogicalOp LogicalOp);

		void TestLogicalOpImmediate(
			ELogicalOp LogicalOp);

		void TestLogicalOpZeroPage(
			ELogicalOp LogicalOp);

		void TestLogicalOpZeroPageX(
			ELogicalOp LogicalOp);

		void TestLogicalOpAbsolute(
			ELogicalOp LogicalOp);

		void TestLogicalOpAbsoluteX(
			ELogicalOp LogicalOp);

		void TestLogicalOpAbsoluteY(
			ELogicalOp LogicalOp);

		void TestLoadRegisterAbsoluteYWhenCrossingPage(
			ELogicalOp LogicalOp);

		void TestLoadRegisterAbsoluteXWhenCrossingPage(
			ELogicalOp LogicalOp);

		void TestLogicalOpIndirectX(
			ELogicalOp LogicalOp);

		void TestLogicalOpIndirectY(
			ELogicalOp LogicalOp);

		void TestLogicalOpIndirectYWhenItCrossesAPage(
			ELogicalOp LogicalOp);

		void TestLogicalOpZeroPageXWhenItWraps(
			ELogicalOp LogicalOp);
	};

	Byte M6502AndEorOraTests::DoLogicalOp(
		Byte A,
		Byte B,
		ELogicalOp LogicalOp)
	{
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				return static_cast<Byte>(A & B);
			case ELogicalOp::Or:
				return static_cast<Byte>(A | B);
			case ELogicalOp::Eor:
				return static_cast<Byte>(A ^ B);
		}

		throw - 1; //invalid Logical Op
	}

	void M6502AndEorOraTests::TestLogicalOpImmediate(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.Accumulator = 0xCC;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_IMMEDIATE;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_IMMEDIATE;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_IMMEDIATE;
				break;
		}
		mem[0xFFFD] = 0x84;

		//when:
		CPU CPUCopy = cpu;
		cpu.Execute(2, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x84, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpZeroPage(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.Accumulator = 0xCC;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ZERO_PAGE;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ZERO_PAGE;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ZERO_PAGE;
				break;
		}
		mem[0xFFFD] = 0x42;
		mem[0x0042] = 0x37;

		//when:
		CPU CPUCopy = cpu;
		cpu.Execute(3, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpZeroPageX(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 5;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ZERO_PAGE_X;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ZERO_PAGE_X;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ZERO_PAGE_X;
				break;
		}
		mem[0xFFFD] = 0x42;
		mem[0x0047] = 0x37;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(4, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpAbsolute(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.Accumulator = 0xCC;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ABSOLUTE;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ABSOLUTE;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ABSOLUTE;
				break;
		}
		mem[0xFFFD] = 0x80;
		mem[0xFFFE] = 0x44;	//0x4480
		mem[0x4480] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpAbsoluteX(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 1;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ABSOLUTE_X;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ABSOLUTE_X;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ABSOLUTE_X;
				break;
		}
		mem[0xFFFD] = 0x80;
		mem[0xFFFE] = 0x44;	//0x4480
		mem[0x4481] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpAbsoluteY(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 1;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ABSOLUTE_Y;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ABSOLUTE_Y;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ABSOLUTE_Y;
				break;
		}
		mem[0xFFFD] = 0x80;
		mem[0xFFFE] = 0x44;	//0x4480
		mem[0x4481] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLoadRegisterAbsoluteYWhenCrossingPage(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0xFF;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ABSOLUTE_Y;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ABSOLUTE_Y;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ABSOLUTE_Y;
				break;
		}
		mem[0xFFFD] = 0x02;
		mem[0xFFFE] = 0x44;	//0x4402
		mem[0x4501] = 0x37;	//0x4402+0xFF crosses page boundary!
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLoadRegisterAbsoluteXWhenCrossingPage(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ABSOLUTE_X;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ABSOLUTE_X;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ABSOLUTE_X;
				break;
		}
		mem[0xFFFD] = 0x02;
		mem[0xFFFE] = 0x44;	//0x4402
		mem[0x4501] = 0x37;	//0x4402+0xFF crosses page boundary!
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpIndirectX(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0x04;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_INDIRECT_X;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_INDIRECT_X;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_INDIRECT_X;
				break;
		}
		mem[0xFFFD] = 0x02;
		mem[0x0006] = 0x00;	//0x2 + 0x4
		mem[0x0007] = 0x80;
		mem[0x8000] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpIndirectY(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0x04;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_INDIRECT_Y;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_INDIRECT_Y;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_INDIRECT_Y;
				break;
		}
		mem[0xFFFD] = 0x02;
		mem[0x0002] = 0x00;
		mem[0x0003] = 0x80;
		mem[0x8004] = 0x37;	//0x8000 + 0x4
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpIndirectYWhenItCrossesAPage(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.Accumulator = 0xCC;
		cpu.YRegister = 0xFF;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_INDIRECT_Y;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_INDIRECT_Y;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_INDIRECT_Y;
				break;
		}
		mem[0xFFFD] = 0x02;
		mem[0x0002] = 0x02;
		mem[0x0003] = 0x80;
		mem[0x8101] = 0x37;	//0x8002 + 0xFF
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	void M6502AndEorOraTests::TestLogicalOpZeroPageXWhenItWraps(
		ELogicalOp LogicalOp)
	{
		// given:
		cpu.Accumulator = 0xCC;
		cpu.XRegister = 0xFF;
		switch (LogicalOp)
		{
			case ELogicalOp::And:
				mem[0xFFFC] = Opcode::AND_ZERO_PAGE_X;
				break;
			case ELogicalOp::Or:
				mem[0xFFFC] = Opcode::ORA_ZERO_PAGE_X;
				break;
			case ELogicalOp::Eor:
				mem[0xFFFC] = Opcode::EOR_ZERO_PAGE_X;
				break;
		}
		mem[0xFFFD] = 0x80;
		mem[0x007F] = 0x37;

		//when:
		CPU CPUCopy = cpu;
		cpu.Execute(4, mem);

		//then:
		const Byte ExpectedResult = DoLogicalOp(0xCC, 0x37, LogicalOp);
		const bool ExpectedNegative = (ExpectedResult & 0b10000000) > 0;
		EXPECT_EQ(cpu.Accumulator, ExpectedResult);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_EQ(cpu.GetFlag(Flag::Negative), ExpectedNegative);
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpANDOnARegisterImmediate)
	{
		TestLogicalOpImmediate(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOROnARegisterImmediate)
	{
		TestLogicalOpImmediate(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEOROnARegisterImmediate)
	{
		TestLogicalOpImmediate(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndOnARegisterZeroPage)
	{
		TestLogicalOpZeroPage(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrOnARegisterZeroPage)
	{
		TestLogicalOpZeroPage(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorOnARegisterZeroPage)
	{
		TestLogicalOpZeroPage(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorImmediateCanAffectZeroFlag)
	{
		// given:
		cpu.Accumulator = 0xCC;
		mem[0xFFFC] = Opcode::EOR_IMMEDIATE;
		mem[0xFFFD] = cpu.Accumulator;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(2, mem);

		//then:
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLogicalOpInstruction(cpu, CPUCopy);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndOnARegisterZeroPageX)
	{
		TestLogicalOpZeroPageX(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrOnARegisterZeroPageX)
	{
		TestLogicalOpZeroPageX(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorOnARegisterZeroPageX)
	{
		TestLogicalOpZeroPageX(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, LogicalOpEorCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX)
	{
		TestLogicalOpZeroPageXWhenItWraps(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, LogicalOpOrCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX)
	{
		TestLogicalOpZeroPageXWhenItWraps(ELogicalOp::Or);
	}
	TEST_F(M6502AndEorOraTests, LogicalOpAndCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX)
	{
		TestLogicalOpZeroPageXWhenItWraps(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorOnARegisterAbsolute)
	{
		TestLogicalOpAbsolute(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrOnARegisterAbsolute)
	{
		TestLogicalOpAbsolute(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndOnARegisterAbsolute)
	{
		TestLogicalOpAbsolute(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndOnARegisterAbsoluteX)
	{
		TestLogicalOpAbsoluteX(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrOnARegisterAbsoluteX)
	{
		TestLogicalOpAbsoluteX(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorOnARegisterAbsoluteX)
	{
		TestLogicalOpAbsoluteX(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndWhenItCrossesAPageBoundaryAbsoluteX)
	{
		TestLoadRegisterAbsoluteXWhenCrossingPage(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrWhenItCrossesAPageBoundaryAbsoluteX)
	{
		TestLoadRegisterAbsoluteXWhenCrossingPage(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorWhenItCrossesAPageBoundaryAbsoluteX)
	{
		TestLoadRegisterAbsoluteXWhenCrossingPage(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndAbsoluteY)
	{
		TestLogicalOpAbsoluteY(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrAbsoluteY)
	{
		TestLogicalOpAbsoluteY(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorAbsoluteY)
	{
		TestLogicalOpAbsoluteY(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndWhenItCrossesAPageBoundaryAbsoluteY)
	{
		TestLoadRegisterAbsoluteYWhenCrossingPage(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrWhenItCrossesAPageBoundaryAbsoluteY)
	{
		TestLoadRegisterAbsoluteYWhenCrossingPage(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorWhenItCrossesAPageBoundaryAbsoluteY)
	{
		TestLoadRegisterAbsoluteYWhenCrossingPage(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndIndirectX)
	{
		TestLogicalOpIndirectX(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorIndirectX)
	{
		TestLogicalOpIndirectX(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrIndirectX)
	{
		TestLogicalOpIndirectX(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndIndirectY)
	{
		TestLogicalOpIndirectY(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrIndirectY)
	{
		TestLogicalOpIndirectY(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorIndirectY)
	{
		TestLogicalOpIndirectY(ELogicalOp::Eor);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpAndWhenItCrossesAPageIndirectY)
	{
		TestLogicalOpIndirectYWhenItCrossesAPage(ELogicalOp::And);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpOrWhenItCrossesAPageIndirectY)
	{
		TestLogicalOpIndirectYWhenItCrossesAPage(ELogicalOp::Or);
	}

	TEST_F(M6502AndEorOraTests, TestLogicalOpEorWhenItCrossesAPageIndirectY)
	{
		TestLogicalOpIndirectYWhenItCrossesAPage(ELogicalOp::Eor);
	}

}

#endif
