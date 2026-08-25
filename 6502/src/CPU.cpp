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

	std::uint16_t CPU::ReadWord(std::uint32_t& cycles, Memory& memory, std::uint16_t address)
	{
		std::uint16_t word{ static_cast<std::uint16_t>(memory[address] | (memory[address + 1] << 8)) };
		cycles -= 2;
		return word;
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

				case Opcode::LDA_ABSOLUTE:
				{
					std::uint16_t absoluteAddress{ FetchWord(cycles, memory) };
					Accumulator = ReadByte(cycles, memory, absoluteAddress);
					LDASetStatus();
					break;
				}

				case Opcode::LDA_ABSOLUTE_X:
				{
					std::uint16_t absoluteAddress{ FetchWord(cycles, memory) };
					std::uint16_t effectiveAddress{ static_cast<std::uint16_t>(absoluteAddress + XRegister) };
					if ((absoluteAddress & 0xFF00) != (effectiveAddress & 0xFF00))
					{
						cycles--;
					}
					Accumulator = ReadByte(cycles, memory, effectiveAddress);
					LDASetStatus();
					break;
				}

				case Opcode::LDA_ABSOLUTE_Y:
				{
					std::uint16_t absoluteAddress{ FetchWord(cycles, memory) };
					std::uint16_t effectiveAddress{ static_cast<std::uint16_t>(absoluteAddress + YRegister) };
					if ((absoluteAddress & 0xFF00) != (effectiveAddress & 0xFF00))
					{
						cycles--;
					}
					Accumulator = ReadByte(cycles, memory, effectiveAddress);
					LDASetStatus();
					break;
				}

				case Opcode::LDA_INDIRECT_X:
				{
					std::uint8_t zeroPageAddress{ static_cast<std::uint8_t>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					std::uint16_t effectiveAddress{ ReadWord(cycles, memory, zeroPageAddress) };
					Accumulator = ReadByte(cycles, memory, effectiveAddress);
					LDASetStatus();
					break;
				}

				case Opcode::LDA_INDIRECT_Y:
				{
					std::uint8_t zeroPageAddress{ FetchByte(cycles, memory) };
					std::uint16_t effectiveAddress{ ReadWord(cycles, memory, zeroPageAddress) };
					std::uint16_t finalAddress{ static_cast<std::uint16_t>(effectiveAddress + YRegister) };
					if ((effectiveAddress & 0xFF00) != (finalAddress & 0xFF00))
					{
						cycles--;
					}
					Accumulator = ReadByte(cycles, memory, finalAddress);
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
