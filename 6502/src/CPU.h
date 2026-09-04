#pragma once

#include "Types.h"

#include "Memory.h"

namespace MOS6502 {

	namespace Opcode {
		static constexpr Byte LDA_IMMEDIATE{ 0xA9 };
		static constexpr Byte LDA_ZERO_PAGE{ 0xA5 };
		static constexpr Byte LDA_ZERO_PAGE_X{ 0xB5 };
		static constexpr Byte LDA_ABSOLUTE{ 0xAD };
		static constexpr Byte LDA_ABSOLUTE_X{ 0xBD };
		static constexpr Byte LDA_ABSOLUTE_Y{ 0xB9 };
		static constexpr Byte LDA_INDIRECT_X{ 0xA1 };
		static constexpr Byte LDA_INDIRECT_Y{ 0xB1 };

		static constexpr Byte LDX_IMMEDIATE{ 0xA2 };
		static constexpr Byte LDX_ZERO_PAGE{ 0xA6 };
		static constexpr Byte LDX_ZERO_PAGE_Y{ 0xB6 };
		static constexpr Byte LDX_ABSOLUTE{ 0xAE };
		static constexpr Byte LDX_ABSOLUTE_Y{ 0xBE };

		static constexpr Byte LDY_IMMEDIATE{ 0xA0 };
		static constexpr Byte LDY_ZERO_PAGE{ 0xA4 };
		static constexpr Byte LDY_ZERO_PAGE_X{ 0xB4 };
		static constexpr Byte LDY_ABSOLUTE{ 0xAC };
		static constexpr Byte LDY_ABSOLUTE_X{ 0xBC };

		static constexpr Byte STA_ZERO_PAGE{ 0x85 };
		static constexpr Byte STA_ZERO_PAGE_X{ 0x95 };
		static constexpr Byte STA_ABSOLUTE{ 0x8D };
		static constexpr Byte STA_ABSOLUTE_X{ 0x9D };
		static constexpr Byte STA_ABSOLUTE_Y{ 0x99 };
		static constexpr Byte STA_INDIRECT_X{ 0x81 };
		static constexpr Byte STA_INDIRECT_Y{ 0x91 };

		static constexpr Byte STX_ZERO_PAGE{ 0x86 };
		static constexpr Byte STX_ZERO_PAGE_Y{ 0x96 };
		static constexpr Byte STX_ABSOLUTE{ 0x8E };

		static constexpr Byte STY_ZERO_PAGE{ 0x84 };
		static constexpr Byte STY_ZERO_PAGE_X{ 0x94 };
		static constexpr Byte STY_ABSOLUTE{ 0x8C };

		static constexpr Byte TAX{ 0xAA };
		static constexpr Byte TAY{ 0xA8 };
		static constexpr Byte TXA{ 0x8A };
		static constexpr Byte TYA{ 0x98 };

		static constexpr Byte AND_IMMEDIATE{ 0x29 };
		static constexpr Byte AND_ZERO_PAGE{ 0x25 };
		static constexpr Byte AND_ZERO_PAGE_X{ 0x35 };
		static constexpr Byte AND_ABSOLUTE{ 0x2D };
		static constexpr Byte AND_ABSOLUTE_X{ 0x3D };
		static constexpr Byte AND_ABSOLUTE_Y{ 0x39 };
		static constexpr Byte AND_INDIRECT_X{ 0x21 };
		static constexpr Byte AND_INDIRECT_Y{ 0x31 };

		static constexpr Byte EOR_IMMEDIATE{ 0x49 };
		static constexpr Byte EOR_ZERO_PAGE{ 0x45 };
		static constexpr Byte EOR_ZERO_PAGE_X{ 0x55 };
		static constexpr Byte EOR_ABSOLUTE{ 0x4D };
		static constexpr Byte EOR_ABSOLUTE_X{ 0x5D };
		static constexpr Byte EOR_ABSOLUTE_Y{ 0x59 };
		static constexpr Byte EOR_INDIRECT_X{ 0x41 };
		static constexpr Byte EOR_INDIRECT_Y{ 0x51 };

		static constexpr Byte ORA_IMMEDIATE{ 0x09 };
		static constexpr Byte ORA_ZERO_PAGE{ 0x05 };
		static constexpr Byte ORA_ZERO_PAGE_X{ 0x15 };
		static constexpr Byte ORA_ABSOLUTE{ 0x0D };
		static constexpr Byte ORA_ABSOLUTE_X{ 0x1D };
		static constexpr Byte ORA_ABSOLUTE_Y{ 0x19 };
		static constexpr Byte ORA_INDIRECT_X{ 0x01 };
		static constexpr Byte ORA_INDIRECT_Y{ 0x11 };

		static constexpr Byte BIT_ZERO_PAGE{ 0x24 };
		static constexpr Byte BIT_ABSOLUTE{ 0x2C };

		static constexpr Byte INC_ZERO_PAGE{ 0xE6 };
		static constexpr Byte INC_ZERO_PAGE_X{ 0xF6 };
		static constexpr Byte INC_ABSOLUTE{ 0xEE };
		static constexpr Byte INC_ABSOLUTE_X{ 0xFE };
		static constexpr Byte INX{ 0xE8 };
		static constexpr Byte INY{ 0xC8 };

		static constexpr Byte DEC_ZERO_PAGE{ 0xC6 };
		static constexpr Byte DEC_ZERO_PAGE_X{ 0xD6 };
		static constexpr Byte DEC_ABSOLUTE{ 0xCE };
		static constexpr Byte DEC_ABSOLUTE_X{ 0xDE };
		static constexpr Byte DEX{ 0xCA };
		static constexpr Byte DEY{ 0x88 };

		static constexpr Byte TSX{ 0xBA };
		static constexpr Byte TXS{ 0x9A };
		static constexpr Byte PHA{ 0x48 };
		static constexpr Byte PHP{ 0x08 };
		static constexpr Byte PLA{ 0x68 };
		static constexpr Byte PLP{ 0x28 };

		static constexpr Byte JMP_ABSOLUTE{ 0x4C };
		static constexpr Byte JMP_INDIRECT{ 0x6C };
		static constexpr Byte JSR{ 0x20 };
		static constexpr Byte RTS{ 0x60 };
	}

	namespace Flag {

		static constexpr Byte Carry{ 1 << 0 };
		static constexpr Byte Zero{ 1 << 1 };
		static constexpr Byte InterruptDisable{ 1 << 2 };
		static constexpr Byte Decimal{ 1 << 3 };
		static constexpr Byte Break{ 1 << 4 };
		static constexpr Byte Unused{ 1 << 5 };
		static constexpr Byte Overflow{ 1 << 6 };
		static constexpr Byte Negative{ 1 << 7 };

	}

	struct CPU {
		Word ProgramCounter;
		Byte StackPointer;
		Byte Accumulator;
		Byte XRegister;
		Byte YRegister;
		Byte ProcessorStatus;

		bool GetFlag(Byte flag) const;
		void SetFlag(Byte flag, bool value);
		Address GetStackAddress() const;
		void LoadRegisterSetStatus(Byte registerValue);
		void LoadProgram(Memory& memory, Byte* program, std::uint32_t programSize);

		void Reset(Memory& memory);
		Byte FetchByte(Cycles& cycles, Memory& memory);
		Word FetchWord(Cycles& cycles, Memory& memory);
		Byte ReadByte(Cycles& cycles, Memory& memory, Address address);
		Word ReadWord(Cycles& cycles, Memory& memory, Address address);
		Word ReadWordPageWrapped(Cycles& cycles, Memory& memory, Address address);
		void WriteByte(Cycles& cycles, Memory& memory, Address address, Byte value);
		void WriteWord(Cycles& cycles, Memory& memory, Address address, Word value);
		Address AddIndexed(Cycles& cycles, Address address, Byte offset);
		void PushByteToStack(Cycles& cycles, Memory& memory, Byte value);
		void PushProgramCounterToStack(Cycles& cycles, Memory& memory);
		Byte PopByteFromStack(Cycles& cycles, Memory& memory);
		Address PopAddressFromStack(Cycles& cycles, Memory& memory);
		void Execute(Cycles cycles, Memory& memory);
	};

}
