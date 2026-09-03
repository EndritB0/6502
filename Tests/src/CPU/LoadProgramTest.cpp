#include "pch.h"

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	class LoadProgramTest : public CPUTest {
	protected:
		void Load()
		{
			cpu.LoadProgram(memory, program, ProgramSize);
		}

		std::uint32_t CountNonZeroBytes() const
		{
			std::uint32_t nonZeroBytes{ 0 };
			for (std::uint32_t address{}; address < Memory::MemorySize; address++)
			{
				if (memory[static_cast<Address>(address)] != 0x00)
				{
					nonZeroBytes++;
				}
			}
			return nonZeroBytes;
		}

		static constexpr std::uint32_t ProgramSize{ 14 };
		static constexpr Address LoadAddress{ 0x1000 };
		static constexpr Address LastProgramAddress{ 0x100B };
		static constexpr Address LoopTarget{ 0x1002 };

		// First 2 bytes of the program is the load address in little endian
		// $1000  A9 FF     LDA #$FF
		// $1002  85 90     STA $90
		// $1004  8D 00 80  STA $8000
		// $1007  49 CC     EOR #$CC
		// $1009  4C 02 10  JMP $1002
		Byte program[ProgramSize]{
			0x00, 0x10,
			Opcode::LDA_IMMEDIATE, 0xFF,
			Opcode::STA_ZERO_PAGE, 0x90,
			Opcode::STA_ABSOLUTE, 0x00, 0x80,
			Opcode::EOR_IMMEDIATE, 0xCC,
			Opcode::JMP_ABSOLUTE, 0x02, 0x10,
		};
	};

	class LoadProgramGuardTest : public LoadProgramTest {};

	class LoadedProgramExecutionTest : public LoadProgramTest {
	protected:
		void SetUp() override
		{
			CPUTest::SetUp();
			Load();
			cpu.ProgramCounter = LoadAddress;
		}

		static constexpr Cycles FirstPassCycles{ 2 + 3 + 4 + 2 + 3 };
		static constexpr Cycles LoopPassCycles{ 3 + 4 + 2 + 3 };
	};

	TEST_F(LoadProgramTest, LoadsEveryProgramByteAtTheHeaderAddress)
	{
		Load();

		EXPECT_EQ(memory[0x1000], Opcode::LDA_IMMEDIATE);
		EXPECT_EQ(memory[0x1001], 0xFF);
		EXPECT_EQ(memory[0x1002], Opcode::STA_ZERO_PAGE);
		EXPECT_EQ(memory[0x1003], 0x90);
		EXPECT_EQ(memory[0x1004], Opcode::STA_ABSOLUTE);
		EXPECT_EQ(memory[0x1005], 0x00);
		EXPECT_EQ(memory[0x1006], 0x80);
		EXPECT_EQ(memory[0x1007], Opcode::EOR_IMMEDIATE);
		EXPECT_EQ(memory[0x1008], 0xCC);
		EXPECT_EQ(memory[0x1009], Opcode::JMP_ABSOLUTE);
		EXPECT_EQ(memory[0x100A], 0x02);
		EXPECT_EQ(memory[0x100B], 0x10);
	}

	TEST_F(LoadProgramTest, ReadsTheLoadAddressLittleEndian)
	{
		Load();

		EXPECT_EQ(memory[LoadAddress], Opcode::LDA_IMMEDIATE);
		EXPECT_EQ(memory[0x0010], 0x00);
	}

	TEST_F(LoadProgramTest, DoesNotCopyTheHeaderBytes)
	{
		Load();

		EXPECT_EQ(memory[0x1000], Opcode::LDA_IMMEDIATE);
		EXPECT_EQ(memory[0x1001], 0xFF);
		EXPECT_EQ(memory[0x100C], 0x00);
	}

	TEST_F(LoadProgramTest, LeavesTheBytesAroundTheProgramUntouched)
	{
		Load();

		EXPECT_EQ(memory[0x0FFF], 0x00);
		EXPECT_EQ(memory[0x100C], 0x00);
	}

	TEST_F(LoadProgramTest, OverwritesWhateverWasAlreadyThere)
	{
		for (Address address{ LoadAddress }; address <= LastProgramAddress; address++)
		{
			memory[address] = 0x42;
		}

		Load();

		EXPECT_EQ(memory[LoadAddress], Opcode::LDA_IMMEDIATE);
		EXPECT_EQ(memory[0x1005], 0x00);
		EXPECT_EQ(memory[LastProgramAddress], 0x10);
	}

	TEST_F(LoadProgramTest, LoadsAtTheAddressTheHeaderNames)
	{
		program[0] = 0x00;
		program[1] = 0x20;

		Load();

		EXPECT_EQ(memory[0x2000], Opcode::LDA_IMMEDIATE);
		EXPECT_EQ(memory[0x200B], 0x10);
		EXPECT_EQ(memory[LoadAddress], 0x00);
	}

	TEST_F(LoadProgramTest, LoadsAProgramIntoTheZeroPage)
	{
		program[0] = 0x80;
		program[1] = 0x00;

		Load();

		EXPECT_EQ(memory[0x0080], Opcode::LDA_IMMEDIATE);
		EXPECT_EQ(memory[0x008B], 0x10);
	}

	TEST_F(LoadProgramGuardTest, NullProgramWritesNothing)
	{
		cpu.LoadProgram(memory, nullptr, ProgramSize);

		EXPECT_EQ(CountNonZeroBytes(), 0u);
	}

	TEST_F(LoadProgramGuardTest, EmptyProgramWritesNothing)
	{
		cpu.LoadProgram(memory, program, 0);

		EXPECT_EQ(CountNonZeroBytes(), 0u);
	}

	TEST_F(LoadProgramGuardTest, ProgramShorterThanTheHeaderWritesNothing)
	{
		cpu.LoadProgram(memory, program, 1);

		EXPECT_EQ(CountNonZeroBytes(), 0u);
	}

	TEST_F(LoadProgramGuardTest, HeaderOnlyProgramWritesNothing)
	{
		cpu.LoadProgram(memory, program, 2);

		EXPECT_EQ(CountNonZeroBytes(), 0u);
	}

	TEST_F(LoadedProgramExecutionTest, RunsTheFirstInstruction)
	{
		cpu.Execute(2, memory);

		EXPECT_EQ(cpu.Accumulator, 0xFF);
		EXPECT_EQ(cpu.ProgramCounter, 0x1002);
	}

	TEST_F(LoadedProgramExecutionTest, StoresTheAccumulatorToBothTargets)
	{
		cpu.Execute(2 + 3 + 4, memory);

		EXPECT_EQ(memory[0x0090], 0xFF);
		EXPECT_EQ(memory[0x8000], 0xFF);
		EXPECT_EQ(cpu.ProgramCounter, 0x1007);
	}

	TEST_F(LoadedProgramExecutionTest, EorsTheAccumulatorWithTheOperand)
	{
		cpu.Execute(2 + 3 + 4 + 2, memory);

		EXPECT_EQ(cpu.Accumulator, 0x33);
		EXPECT_EQ(cpu.ProgramCounter, 0x1009);
	}

	TEST_F(LoadedProgramExecutionTest, JumpsBackToTheStoreInstruction)
	{
		cpu.Execute(FirstPassCycles, memory);

		EXPECT_EQ(cpu.ProgramCounter, LoopTarget);
	}

	TEST_F(LoadedProgramExecutionTest, AlternatesTheAccumulatorOnTheSecondPass)
	{
		cpu.Execute(FirstPassCycles + LoopPassCycles, memory);

		EXPECT_EQ(cpu.Accumulator, 0xFF);
		EXPECT_EQ(memory[0x0090], 0x33);
		EXPECT_EQ(memory[0x8000], 0x33);
		EXPECT_EQ(cpu.ProgramCounter, LoopTarget);
	}

	TEST_F(LoadedProgramExecutionTest, SetsTheNegativeFlagFromTheEorResult)
	{
		cpu.Execute(FirstPassCycles, memory);

		EXPECT_FALSE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));

		cpu.Execute(LoopPassCycles, memory);

		EXPECT_TRUE(cpu.GetFlag(Flag::Negative));
		EXPECT_FALSE(cpu.GetFlag(Flag::Zero));
	}

	TEST_F(LoadedProgramExecutionTest, LeavesTheIndexRegistersAndStackAlone)
	{
		cpu.Execute(FirstPassCycles, memory);

		EXPECT_EQ(cpu.XRegister, 0x00);
		EXPECT_EQ(cpu.YRegister, 0x00);
		EXPECT_EQ(cpu.StackPointer, InitialStackPointer);
	}

}
