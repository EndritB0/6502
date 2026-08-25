#include "pch.h"

#include <CPU.h>

#include <cstdint>

namespace Test6502 {

	using MOS6502::CPU;
	using MOS6502::Memory;

	class MemoryTest : public ::testing::Test {
	protected:
		void SetUp() override
		{
			memory.Init();
		}

		void FillWithNonZeroPattern()
		{
			for (std::uint32_t address{}; address < Memory::MemorySize; address++)
			{
				memory[address] = static_cast<std::uint8_t>((address % 255) + 1);
			}
		}

		std::uint32_t CountNonZeroBytes() const
		{
			std::uint32_t nonZeroBytes{ 0 };
			for (std::uint32_t address{}; address < Memory::MemorySize; address++)
			{
				if (memory[address] != 0x00)
				{
					nonZeroBytes++;
				}
			}
			return nonZeroBytes;
		}

		Memory memory;
	};

	class MemoryWriteWordTest : public MemoryTest {};

	class CPUMemoryAccessTest : public ::testing::Test {
	protected:
		void SetUp() override
		{
			cpu.Reset(memory);
		}

		CPU cpu;
		Memory memory;
	};

	TEST(MemoryLayoutTest, MemoryIsSixtyFourKilobytes)
	{
		EXPECT_EQ(Memory::MemorySize, 1024u * 64u);
	}

	TEST_F(MemoryTest, InitZeroesEveryByte)
	{
		FillWithNonZeroPattern();

		memory.Init();

		EXPECT_EQ(CountNonZeroBytes(), 0u);
	}

	TEST_F(MemoryTest, InitZeroesTheFirstAndLastByte)
	{
		memory[0x0000] = 0x42;
		memory[0xFFFF] = 0x37;

		memory.Init();

		EXPECT_EQ(memory[0x0000], 0x00);
		EXPECT_EQ(memory[0xFFFF], 0x00);
	}

	TEST_F(MemoryTest, StoresAndReturnsAByte)
	{
		memory[0x1234] = 0x42;

		EXPECT_EQ(memory[0x1234], 0x42);
	}

	TEST_F(MemoryTest, StoresAByteAtTheFirstAndLastAddress)
	{
		memory[0x0000] = 0x42;
		memory[0xFFFF] = 0x37;

		EXPECT_EQ(memory[0x0000], 0x42);
		EXPECT_EQ(memory[0xFFFF], 0x37);
	}

	TEST_F(MemoryTest, KeepsAdjacentAddressesIndependent)
	{
		memory[0x1233] = 0x11;
		memory[0x1234] = 0x22;
		memory[0x1235] = 0x33;

		EXPECT_EQ(memory[0x1233], 0x11);
		EXPECT_EQ(memory[0x1234], 0x22);
		EXPECT_EQ(memory[0x1235], 0x33);
	}

	TEST_F(MemoryTest, ReadsThroughAConstReference)
	{
		memory[0x1234] = 0x42;

		const Memory& constMemory{ memory };

		EXPECT_EQ(constMemory[0x1234], 0x42);
	}

	TEST_F(MemoryTest, HoldsSixtyFourKilobytesOfDistinctBytes)
	{
		FillWithNonZeroPattern();

		EXPECT_EQ(CountNonZeroBytes(), Memory::MemorySize);
	}

	TEST_F(MemoryWriteWordTest, WritesTheLowByteFirst)
	{
		std::uint32_t cycles{ 10 };

		memory.WriteWord(cycles, 0xABCD, 0x1234);

		EXPECT_EQ(memory[0x1234], 0xCD);
		EXPECT_EQ(memory[0x1235], 0xAB);
	}

	TEST_F(MemoryWriteWordTest, ConsumesTwoCycles)
	{
		std::uint32_t cycles{ 10 };

		memory.WriteWord(cycles, 0xABCD, 0x1234);

		EXPECT_EQ(cycles, 8u);
	}

	TEST_F(MemoryWriteWordTest, LeavesTheNeighbouringBytesAlone)
	{
		memory[0x1233] = 0x11;
		memory[0x1236] = 0x22;
		std::uint32_t cycles{ 10 };

		memory.WriteWord(cycles, 0xABCD, 0x1234);

		EXPECT_EQ(memory[0x1233], 0x11);
		EXPECT_EQ(memory[0x1236], 0x22);
	}

	TEST_F(MemoryWriteWordTest, WritesEveryBitOfTheValue)
	{
		std::uint32_t cycles{ 10 };

		memory.WriteWord(cycles, 0xFFFF, 0x1234);

		EXPECT_EQ(memory[0x1234], 0xFF);
		EXPECT_EQ(memory[0x1235], 0xFF);
	}

	TEST_F(MemoryWriteWordTest, WritesZeroOverExistingBytes)
	{
		memory[0x1234] = 0xAA;
		memory[0x1235] = 0xBB;
		std::uint32_t cycles{ 10 };

		memory.WriteWord(cycles, 0x0000, 0x1234);

		EXPECT_EQ(memory[0x1234], 0x00);
		EXPECT_EQ(memory[0x1235], 0x00);
	}

	TEST_F(CPUMemoryAccessTest, ResetZeroesMemory)
	{
		memory[0x0000] = 0x42;
		memory[0x1234] = 0x42;
		memory[0xFFFF] = 0x42;

		cpu.Reset(memory);

		EXPECT_EQ(memory[0x0000], 0x00);
		EXPECT_EQ(memory[0x1234], 0x00);
		EXPECT_EQ(memory[0xFFFF], 0x00);
	}

	TEST_F(CPUMemoryAccessTest, FetchByteReturnsTheByteUnderTheProgramCounter)
	{
		cpu.ProgramCounter = 0x1234;
		memory[0x1234] = 0x42;
		std::uint32_t cycles{ 10 };

		EXPECT_EQ(cpu.FetchByte(cycles, memory), 0x42);
		EXPECT_EQ(cpu.ProgramCounter, 0x1235);
		EXPECT_EQ(cycles, 9u);
	}

	TEST_F(CPUMemoryAccessTest, FetchWordCombinesTwoBytesLittleEndian)
	{
		cpu.ProgramCounter = 0x1234;
		memory[0x1234] = 0xCD;
		memory[0x1235] = 0xAB;
		std::uint32_t cycles{ 10 };

		EXPECT_EQ(cpu.FetchWord(cycles, memory), 0xABCD);
		EXPECT_EQ(cpu.ProgramCounter, 0x1236);
		EXPECT_EQ(cycles, 8u);
	}

	TEST_F(CPUMemoryAccessTest, ReadByteReturnsTheByteAtTheGivenAddress)
	{
		cpu.ProgramCounter = 0x1234;
		memory[0x4242] = 0x37;
		std::uint32_t cycles{ 10 };

		EXPECT_EQ(cpu.ReadByte(cycles, memory, 0x4242), 0x37);
		EXPECT_EQ(cpu.ProgramCounter, 0x1234);
		EXPECT_EQ(cycles, 9u);
	}

	TEST_F(CPUMemoryAccessTest, ReadByteReachesTheFirstAndLastAddress)
	{
		memory[0x0000] = 0x42;
		memory[0xFFFF] = 0x37;
		std::uint32_t cycles{ 10 };

		EXPECT_EQ(cpu.ReadByte(cycles, memory, 0x0000), 0x42);
		EXPECT_EQ(cpu.ReadByte(cycles, memory, 0xFFFF), 0x37);
		EXPECT_EQ(cycles, 8u);
	}

}
