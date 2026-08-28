#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class M6502LoadRegisterTests : public testing::Test {
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

		void TestLoadRegisterImmediate(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterZeroPage(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterZeroPageX(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterZeroPageY(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterAbsolute(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterAbsoluteX(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterAbsoluteY(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterAbsoluteYWhenCrossingPage(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);

		void TestLoadRegisterAbsoluteXWhenCrossingPage(
			Byte OpcodeToTest,
			Byte CPU::* RegisterToTest);
	};

	static void VerfifyUnmodifiedFlagsFromLoadRegister(
		const CPU& cpu,
		const CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.GetFlag(Flag::Carry), CPUCopy.GetFlag(Flag::Carry));
		EXPECT_EQ(cpu.GetFlag(Flag::InterruptDisable), CPUCopy.GetFlag(Flag::InterruptDisable));
		EXPECT_EQ(cpu.GetFlag(Flag::Decimal), CPUCopy.GetFlag(Flag::Decimal));
		EXPECT_EQ(cpu.GetFlag(Flag::Break), CPUCopy.GetFlag(Flag::Break));
		EXPECT_EQ(cpu.GetFlag(Flag::Overflow), CPUCopy.GetFlag(Flag::Overflow));
	}

	TEST_F(M6502LoadRegisterTests, TheCPUDoesNothingWhenWeExecuteZeroCycles)
	{
		//given:
		constexpr Cycles NUM_CYCLES = 0;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(NUM_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.ProgramCounter, CPUCopy.ProgramCounter);
		EXPECT_EQ(cpu.Accumulator, CPUCopy.Accumulator);
		EXPECT_EQ(cpu.XRegister, CPUCopy.XRegister);
		EXPECT_EQ(cpu.YRegister, CPUCopy.YRegister);
		EXPECT_EQ(cpu.ProcessorStatus, CPUCopy.ProcessorStatus);
	}

	TEST_F(M6502LoadRegisterTests, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction)
	{
		// given:
		mem[0xFFFC] = Opcode::LDA_IMMEDIATE;
		mem[0xFFFD] = 0x84;
		constexpr Cycles NUM_CYCLES = 1;

		//when:
		cpu.Execute(NUM_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.Accumulator, 0x84);
	}

	void M6502LoadRegisterTests::TestLoadRegisterImmediate(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x84;

		//when:
		CPU CPUCopy = cpu;
		cpu.Execute(2, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x84);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAImmediateCanLoadAValueIntoTheARegister)
	{
		TestLoadRegisterImmediate(Opcode::LDA_IMMEDIATE, &CPU::Accumulator);
	}

	TEST_F(M6502LoadRegisterTests, LDXImmediateCanLoadAValueIntoTheXRegister)
	{
		TestLoadRegisterImmediate(Opcode::LDX_IMMEDIATE, &CPU::XRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDYImmediateCanLoadAValueIntoTheYRegister)
	{
		TestLoadRegisterImmediate(Opcode::LDY_IMMEDIATE, &CPU::YRegister);
	}

	void M6502LoadRegisterTests::TestLoadRegisterZeroPage(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x42;
		mem[0x0042] = 0x37;

		//when:
		CPU CPUCopy = cpu;
		cpu.Execute(3, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAZeroPageCanLoadAValueIntoTheARegister)
	{
		TestLoadRegisterZeroPage(Opcode::LDA_ZERO_PAGE, &CPU::Accumulator);
	}

	TEST_F(M6502LoadRegisterTests, LDXZeroPageCanLoadAValueIntoTheXRegister)
	{
		TestLoadRegisterZeroPage(Opcode::LDX_ZERO_PAGE, &CPU::XRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDYZeroPageCanLoadAValueIntoTheYRegister)
	{
		TestLoadRegisterZeroPage(Opcode::LDY_ZERO_PAGE, &CPU::YRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDAImmediateCanAffectTheZeroFlag)
	{
		// given:
		cpu.Accumulator = 0x44;
		mem[0xFFFC] = Opcode::LDA_IMMEDIATE;
		mem[0xFFFD] = 0x0;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(2, mem);

		//then:
		EXPECT_TRUE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	void M6502LoadRegisterTests::TestLoadRegisterZeroPageX(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		cpu.XRegister = 5;
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x42;
		mem[0x0047] = 0x37;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(4, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	void M6502LoadRegisterTests::TestLoadRegisterZeroPageY(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		cpu.YRegister = 5;
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x42;
		mem[0x0047] = 0x37;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(4, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegister)
	{
		TestLoadRegisterZeroPageX(Opcode::LDA_ZERO_PAGE_X, &CPU::Accumulator);
	}

	TEST_F(M6502LoadRegisterTests, LDXZeroPageYCanLoadAValueIntoTheXRegister)
	{
		TestLoadRegisterZeroPageY(Opcode::LDX_ZERO_PAGE_Y, &CPU::XRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDYZeroPageXCanLoadAValueIntoTheYRegister)
	{
		TestLoadRegisterZeroPageX(Opcode::LDY_ZERO_PAGE_X, &CPU::YRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps)
	{
		// given:
		cpu.XRegister = 0xFF;
		mem[0xFFFC] = Opcode::LDA_ZERO_PAGE_X;
		mem[0xFFFD] = 0x80;
		mem[0x007F] = 0x37;

		//when:
		CPU CPUCopy = cpu;
		cpu.Execute(4, mem);

		//then:
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	void M6502LoadRegisterTests::TestLoadRegisterAbsolute(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x80;
		mem[0xFFFE] = 0x44;	//0x4480
		mem[0x4480] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAAbsoluteCanLoadAValueIntoTheARegister)
	{
		TestLoadRegisterAbsolute(Opcode::LDA_ABSOLUTE, &CPU::Accumulator);
	}

	TEST_F(M6502LoadRegisterTests, LDXAbsoluteCanLoadAValueIntoTheXRegister)
	{
		TestLoadRegisterAbsolute(Opcode::LDX_ABSOLUTE, &CPU::XRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDYAbsoluteCanLoadAValueIntoTheYRegister)
	{
		TestLoadRegisterAbsolute(Opcode::LDY_ABSOLUTE, &CPU::YRegister);
	}

	void M6502LoadRegisterTests::TestLoadRegisterAbsoluteX(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 1;
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x80;
		mem[0xFFFE] = 0x44;	//0x4480
		mem[0x4481] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	void M6502LoadRegisterTests::TestLoadRegisterAbsoluteY(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.YRegister = 1;
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x80;
		mem[0xFFFE] = 0x44;	//0x4480
		mem[0x4481] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegister)
	{
		TestLoadRegisterAbsoluteX(Opcode::LDA_ABSOLUTE_X, &CPU::Accumulator);
	}

	TEST_F(M6502LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegister)
	{
		TestLoadRegisterAbsoluteY(Opcode::LDX_ABSOLUTE_Y, &CPU::XRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegister)
	{
		TestLoadRegisterAbsoluteX(Opcode::LDY_ABSOLUTE_X, &CPU::YRegister);
	}

	void M6502LoadRegisterTests::TestLoadRegisterAbsoluteXWhenCrossingPage(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		cpu.XRegister = 0xFF;
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x02;
		mem[0xFFFE] = 0x44;	//0x4402
		mem[0x4501] = 0x37;	//0x4402+0xFF crosses page boundary!
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary)
	{
		TestLoadRegisterAbsoluteXWhenCrossingPage(Opcode::LDA_ABSOLUTE_X, &CPU::Accumulator);
	}

	TEST_F(M6502LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegisterWhenItCrossesAPageBoundary)
	{
		TestLoadRegisterAbsoluteXWhenCrossingPage(Opcode::LDY_ABSOLUTE_X, &CPU::YRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegister)
	{
		TestLoadRegisterAbsoluteY(Opcode::LDA_ABSOLUTE_Y, &CPU::Accumulator);
	}

	void M6502LoadRegisterTests::TestLoadRegisterAbsoluteYWhenCrossingPage(
		Byte OpcodeToTest,
		Byte CPU::* RegisterToTest)
	{
		// given:
		cpu.YRegister = 0xFF;
		mem[0xFFFC] = OpcodeToTest;
		mem[0xFFFD] = 0x02;
		mem[0xFFFE] = 0x44;	//0x4402
		mem[0x4501] = 0x37;	//0x4402+0xFF crosses page boundary!
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.*RegisterToTest, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary)
	{
		TestLoadRegisterAbsoluteYWhenCrossingPage(Opcode::LDA_ABSOLUTE_Y, &CPU::Accumulator);
	}

	TEST_F(M6502LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegisterWhenItCrossesAPageBoundary)
	{
		TestLoadRegisterAbsoluteYWhenCrossingPage(Opcode::LDX_ABSOLUTE_Y, &CPU::XRegister);
	}

	TEST_F(M6502LoadRegisterTests, LDAIndirectXCanLoadAValueIntoTheARegister)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.XRegister = 0x04;
		mem[0xFFFC] = Opcode::LDA_INDIRECT_X;
		mem[0xFFFD] = 0x02;
		mem[0x0006] = 0x00;	//0x2 + 0x4
		mem[0x0007] = 0x80;
		mem[0x8000] = 0x37;
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegister)
	{
		// given:
		cpu.SetFlag(Flag::Zero, true);
		cpu.SetFlag(Flag::Negative, true);
		cpu.YRegister = 0x04;
		mem[0xFFFC] = Opcode::LDA_INDIRECT_Y;
		mem[0xFFFD] = 0x02;
		mem[0x0002] = 0x00;
		mem[0x0003] = 0x80;
		mem[0x8004] = 0x37;	//0x8000 + 0x4
		constexpr Cycles EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

	TEST_F(M6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegisterWhenItCrossesAPage)
	{
		// given:
		cpu.YRegister = 0xFF;
		mem[0xFFFC] = Opcode::LDA_INDIRECT_Y;
		mem[0xFFFD] = 0x02;
		mem[0x0002] = 0x02;
		mem[0x0003] = 0x80;
		mem[0x8101] = 0x37;	//0x8002 + 0xFF
		constexpr Cycles EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		//when:
		cpu.Execute(EXPECTED_CYCLES, mem);

		//then:
		EXPECT_EQ(cpu.Accumulator, 0x37);
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		VerfifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
	}

}

#endif
