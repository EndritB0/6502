#pragma once

#include <cstdint>

#include "Memory.h"

namespace MOS6502 {

	namespace Opcode {
		static constexpr std::uint8_t LDA_IMMEDIATE{ 0xA9 };
		static constexpr std::uint8_t LDA_ZERO_PAGE{ 0xA5 };
		static constexpr std::uint8_t LDA_ZERO_PAGE_X{ 0xB5 };

		static constexpr std::uint8_t JSR{ 0x20 };
	}

	namespace Flag {

		static constexpr std::uint8_t Carry{ 1 << 0 };
		static constexpr std::uint8_t Zero{ 1 << 1 };
		static constexpr std::uint8_t InterruptDisable{ 1 << 2 };
		static constexpr std::uint8_t Decimal{ 1 << 3 };
		static constexpr std::uint8_t Break{ 1 << 4 };
		static constexpr std::uint8_t Unused{ 1 << 5 };
		static constexpr std::uint8_t Overflow{ 1 << 6 };
		static constexpr std::uint8_t Negative{ 1 << 7 };

	}

	struct CPU {
		std::uint16_t ProgramCounter;
		std::uint16_t StackPointer;
		std::uint8_t Accumulator;
		std::uint8_t XRegister;
		std::uint8_t YRegister;
		std::uint8_t ProcessorStatus;

		bool GetFlag(std::uint8_t flag) const;
		void SetFlag(std::uint8_t flag, bool value);
		void LDASetStatus();

		void Reset(Memory& memory);
		std::uint8_t FetchByte(std::uint32_t& cycles, Memory& memory);
		std::uint16_t FetchWord(std::uint32_t& cycles, Memory& memory);
		std::uint8_t ReadByte(std::uint32_t& cycles, Memory& memory, std::uint16_t address);
		void Execute(std::uint32_t cycles, Memory& memory);
	};

}
