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

		static constexpr Byte JSR{ 0x20 };
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
		Word StackPointer;
		Byte Accumulator;
		Byte XRegister;
		Byte YRegister;
		Byte ProcessorStatus;

		bool GetFlag(Byte flag) const;
		void SetFlag(Byte flag, bool value);
		void LoadRegisterSetStatus(Byte registerValue);

		void Reset(Memory& memory);
		Byte FetchByte(Cycles& cycles, Memory& memory);
		Word FetchWord(Cycles& cycles, Memory& memory);
		Byte ReadByte(Cycles& cycles, Memory& memory, Address address);
		Word ReadWord(Cycles& cycles, Memory& memory, Address address);
		Address AddIndexed(Cycles& cycles, Address address, Byte offset);
		void Execute(Cycles cycles, Memory& memory);
	};

}
