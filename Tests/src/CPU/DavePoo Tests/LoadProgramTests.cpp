#include "pch.h"

#include "DavePooTests.h"

#if ENABLE_DAVEPOO_TESTS

#include <CPU.h>
#include "CPU/CPUTests.h"

namespace Test6502 {

	/**
	; TestPrg

	* = $1000

	lda #$FF

	start
	sta $90
	sta $8000
	eor #$CC
	jmp start

	*/
	static Byte TestPrg[] = {
		0x00, 0x10, 0xA9, 0xFF, 0x85, 0x90,
		0x8D, 0x00, 0x80, 0x49, 0xCC, 0x4C, 0x02, 0x10 };

	static const std::uint32_t NumBytesInPrg = 14;

	class M6502LoadPrgTests : public testing::Test {
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

		// LoadProgram takes the memory first and returns void, so the reference
		// suite's LoadPrg( Prg, NumBytes, Mem ) -> Word becomes a call with the
		// arguments reordered plus a read of the program's own little endian
		// header for the start address.
		Word LoadPrg(Byte* program, std::uint32_t programSize)
		{
			cpu.LoadProgram(mem, program, programSize);
			return static_cast<Word>(program[0] | (program[1] << 8));
		}
	};

	TEST_F(M6502LoadPrgTests, TestLoadProgramAProgramIntoTheCorrectAreaOfMemory)
	{
		// given:

		// when:
		LoadPrg(TestPrg, NumBytesInPrg);

		//then:
		EXPECT_EQ(mem[0x0FFF], 0x0);
		EXPECT_EQ(mem[0x1000], 0xA9);
		EXPECT_EQ(mem[0x1001], 0xFF);
		EXPECT_EQ(mem[0x1002], 0x85);
		//....
		EXPECT_EQ(mem[0x1009], 0x4C);
		EXPECT_EQ(mem[0x100A], 0x02);
		EXPECT_EQ(mem[0x100B], 0x10);
		EXPECT_EQ(mem[0x100C], 0x0);
	}

	TEST_F(M6502LoadPrgTests, TestLoadProgramAProgramAndExecuteIt)
	{
		// given:

		// when:
		Word StartAddress = LoadPrg(TestPrg, NumBytesInPrg);
		cpu.ProgramCounter = StartAddress;

		//then:
		// Execute returns void here and already runs until its cycle budget is
		// spent, so the reference suite's clock loop becomes a single call.
		cpu.Execute(1000, mem);
	}

}

#endif
