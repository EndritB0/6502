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

	bool CPU::GetFlag(Byte flag) const
	{
		return (ProcessorStatus & flag) != 0;
	}

	void CPU::SetFlag(Byte flag, bool value)
	{
		const int updated{ value ? (ProcessorStatus | flag) : (ProcessorStatus & ~flag) };
		ProcessorStatus = static_cast<Byte>(updated);
	}

	void CPU::LoadRegisterSetStatus(Byte registerValue)
	{
		SetFlag(Flag::Zero, registerValue == 0);
		SetFlag(Flag::Negative, (registerValue & 0x80) != 0);
	}

	Byte CPU::FetchByte(Cycles& cycles, Memory& memory)
	{
		Byte byte{ memory[ProgramCounter] };
		ProgramCounter++;
		cycles--;
		return byte;
	}

	Word CPU::FetchWord(Cycles& cycles, Memory& memory)
	{
		Word word{ static_cast<Word>(memory[ProgramCounter] | (memory[static_cast<Address>(ProgramCounter + 1)] << 8)) };
		ProgramCounter += 2;
		cycles -= 2;
		return word;
	}

	Byte CPU::ReadByte(Cycles& cycles, Memory& memory, Address address)
	{
		Byte byte{ memory[address] };
		cycles--;
		return byte;
	}

	Word CPU::ReadWord(Cycles& cycles, Memory& memory, Address address)
	{
		Word word{ static_cast<Word>(memory[address] | (memory[static_cast<Address>(address + 1)] << 8)) };
		cycles -= 2;
		return word;
	}

	void CPU::WriteByte(Cycles& cycles, Memory& memory, Address address, Byte value)
	{
		memory[address] = value;
		cycles--;
	}

	void CPU::WriteWord(Cycles& cycles, Memory& memory, Address address, Word value)
	{
		memory[address] = static_cast<Byte>(value & 0x00FF);
		memory[static_cast<Address>(address + 1)] = static_cast<Byte>((value & 0xFF00) >> 8);
		cycles -= 2;
	}

	Address CPU::AddIndexed(Cycles& cycles, Address address, Byte offset)
	{
		Address effectiveAddress{ static_cast<Address>(address + offset) };
		if ((address & 0xFF00) != (effectiveAddress & 0xFF00))
		{
			cycles--;
		}
		return effectiveAddress;
	}

	void CPU::Execute(Cycles cycles, Memory& memory)
	{
		while (cycles > 0)
		{
			Byte opcode{ FetchByte(cycles, memory) };

			switch (opcode)
			{
				case Opcode::LDA_IMMEDIATE:
				{
					Byte value{ FetchByte(cycles, memory) };
					Accumulator = value;
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Accumulator = ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Accumulator = ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Accumulator = ReadByte(cycles, memory, absoluteAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, XRegister) };
					Accumulator = ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_ABSOLUTE_Y:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, YRegister) };
					Accumulator = ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_INDIRECT_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Address effectiveAddress{ ReadWord(cycles, memory, zeroPageAddress) };
					Accumulator = ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_INDIRECT_Y:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Address effectiveAddress{ ReadWord(cycles, memory, zeroPageAddress) };
					Address finalAddress{ AddIndexed(cycles, effectiveAddress, YRegister) };
					Accumulator = ReadByte(cycles, memory, finalAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDX_IMMEDIATE:
				{
					Byte value{ FetchByte(cycles, memory) };
					XRegister = value;
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::LDX_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					XRegister = ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::LDX_ZERO_PAGE_Y:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + YRegister) };
					cycles--;
					XRegister = ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::LDX_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					XRegister = ReadByte(cycles, memory, absoluteAddress);
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::LDX_ABSOLUTE_Y:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, YRegister) };
					XRegister = ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::LDY_IMMEDIATE:
				{
					Byte value{ FetchByte(cycles, memory) };
					YRegister = value;
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::LDY_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					YRegister = ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::LDY_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					YRegister = ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::LDY_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					YRegister = ReadByte(cycles, memory, absoluteAddress);
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::LDY_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, XRegister) };
					YRegister = ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::STA_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					WriteByte(cycles, memory, zeroPageAddress, Accumulator);
					break;
				}

				case Opcode::STA_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					WriteByte(cycles, memory, zeroPageAddress, Accumulator);
					break;
				}

				case Opcode::STA_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					WriteByte(cycles, memory, absoluteAddress, Accumulator);
					break;
				}

				case Opcode::STA_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ static_cast<Address>(absoluteAddress + XRegister) };
					cycles--;
					WriteByte(cycles, memory, effectiveAddress, Accumulator);
					break;
				}

				case Opcode::STA_ABSOLUTE_Y:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ static_cast<Address>(absoluteAddress + YRegister) };
					cycles--;
					WriteByte(cycles, memory, effectiveAddress, Accumulator);
					break;
				}

				case Opcode::STA_INDIRECT_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Address effectiveAddress{ ReadWord(cycles, memory, zeroPageAddress) };
					WriteByte(cycles, memory, effectiveAddress, Accumulator);
					break;
				}

				case Opcode::STA_INDIRECT_Y:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Address effectiveAddress{ ReadWord(cycles, memory, zeroPageAddress) };
					Address finalAddress{ static_cast<Address>(effectiveAddress + YRegister) };
					cycles--;
					WriteByte(cycles, memory, finalAddress, Accumulator);
					break;
				}

				case Opcode::STX_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					WriteByte(cycles, memory, zeroPageAddress, XRegister);
					break;
				}

				case Opcode::STX_ZERO_PAGE_Y:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + YRegister) };
					cycles--;
					WriteByte(cycles, memory, zeroPageAddress, XRegister);
					break;
				}

				case Opcode::STX_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					WriteByte(cycles, memory, absoluteAddress, XRegister);
					break;
				}

				case Opcode::STY_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					WriteByte(cycles, memory, zeroPageAddress, YRegister);
					break;
				}

				case Opcode::STY_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					WriteByte(cycles, memory, zeroPageAddress, YRegister);
					break;
				}

				case Opcode::STY_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					WriteByte(cycles, memory, absoluteAddress, YRegister);
					break;
				}

				case Opcode::JSR:
				{
					Address subroutineAddress{ FetchWord(cycles, memory) };
					WriteWord(cycles, memory, StackPointer, static_cast<Word>(ProgramCounter - 1));
					StackPointer += 2;
					ProgramCounter = subroutineAddress;
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
