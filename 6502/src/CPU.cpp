#include "pch.h"

#include "CPU.h"

namespace MOS6502 {

	void CPU::Reset(Memory& memory)
	{
		ProgramCounter = 0xFFFC;
		StackPointer = 0x0100;
		Accumulator = 0x00;
		XRegister = 0x00;
		YRegister = 0x00;
		ProcessorStatus = 0x00;
		memory.Init();
	}

	bool CPU::GetFlag(std::uint8_t flag) const
	{
		return (ProcessorStatus & flag) != 0;
	}

	void CPU::SetFlag(std::uint8_t flag, bool value)
	{
		const int updated{ value ? (ProcessorStatus | flag) : (ProcessorStatus & ~flag) };
		ProcessorStatus = static_cast<std::uint8_t>(updated);
	}

	void CPU::LDASetStatus()
	{
		SetFlag(Flag::Zero, Accumulator == 0);
		SetFlag(Flag::Negative, (Accumulator & 0x80) != 0);
	}

	std::uint8_t CPU::FetchByte(std::uint32_t& cycles, Memory& memory)
	{
		std::uint8_t byte{ memory[ProgramCounter] };
		ProgramCounter++;
		cycles--;
		return byte;
	}

	std::uint16_t CPU::FetchWord(std::uint32_t& cycles, Memory& memory)
	{
		std::uint16_t word{ static_cast<std::uint16_t>(memory[ProgramCounter] | (memory[ProgramCounter + 1] << 8)) };
		ProgramCounter += 2;
		cycles -= 2;
		return word;
	}

	std::uint8_t CPU::ReadByte(std::uint32_t& cycles, Memory& memory, std::uint16_t address)
	{
		std::uint8_t byte{ memory[address] };
		cycles--;
		return byte;
	}

	void CPU::Execute(std::uint32_t cycles, Memory& memory)
	{
		while (cycles > 0)
		{
			std::uint8_t opcode{ FetchByte(cycles, memory) };

			switch (opcode)
			{
				case Opcode::LDA_IMMEDIATE:
				{
					std::uint8_t value{ FetchByte(cycles, memory) };
					Accumulator = value;
					LDASetStatus();
					break;
				}

				case Opcode::LDA_ZERO_PAGE:
				{
					std::uint8_t zeroPageAddress{ FetchByte(cycles, memory) };
					Accumulator = ReadByte(cycles, memory, zeroPageAddress);
					LDASetStatus();
					break;
				}

				case Opcode::LDA_ZERO_PAGE_X:
				{
					std::uint8_t zeroPageAddress{ static_cast<std::uint8_t>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Accumulator = ReadByte(cycles, memory, zeroPageAddress);
					LDASetStatus();
					break;
				}

				case Opcode::JSR:
				{
					std::uint16_t subroutineAddress{ FetchWord(cycles, memory) };
					memory.WriteWord(cycles, ProgramCounter - 1, StackPointer);
					ProgramCounter = subroutineAddress;
					StackPointer += 2;
					cycles--;
					break;
				}

				default:
				{
					// Handle unknown opcode
					break;
				}
			}
		}
	}

}
