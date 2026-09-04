#include "pch.h"

#include "CPU.h"

namespace MOS6502 {

	void CPU::Reset(Memory& memory)
	{
		ProgramCounter = 0xFFFC;
		StackPointer = 0xFF;
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

	Address CPU::GetStackAddress() const
	{
		return static_cast<Address>(0x0100 | StackPointer);
	}

	void CPU::LoadRegisterSetStatus(Byte registerValue)
	{
		SetFlag(Flag::Zero, registerValue == 0);
		SetFlag(Flag::Negative, (registerValue & 0x80) != 0);
	}

	void CPU::LoadProgram(Memory& memory, Byte* program, std::uint32_t programSize)
	{
		if (!program || programSize < 2)
		{
			return;
		}

		std::uint32_t at{ 2 };
		const Address loadAddress{ static_cast<Address>(program[0] | (program[1] << 8)) };
		for (Address i{ loadAddress }; i < loadAddress + programSize - 2; i++)
		{
			memory[i] = program[at++];
		}
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

	Word CPU::ReadWordPageWrapped(Cycles& cycles, Memory& memory, Address address)
	{
		const Address highByteAddress{ static_cast<Address>((address & 0xFF00) | ((address + 1) & 0x00FF)) };
		Word word{ static_cast<Word>(memory[address] | (memory[highByteAddress] << 8)) };
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

	void CPU::PushByteToStack(Cycles& cycles, Memory& memory, Byte value)
	{
		WriteByte(cycles, memory, GetStackAddress(), value);
		StackPointer--;
	}

	void CPU::PushProgramCounterToStack(Cycles& cycles, Memory& memory)
	{
		WriteWord(cycles, memory, GetStackAddress() - 1, ProgramCounter - 1);
		StackPointer -= 2;
	}

	Byte CPU::PopByteFromStack(Cycles& cycles, Memory& memory)
	{
		StackPointer++;
		cycles--;
		return ReadByte(cycles, memory, GetStackAddress());
	}

	Address CPU::PopAddressFromStack(Cycles& cycles, Memory& memory)
	{
		Address address{ ReadWord(cycles, memory, GetStackAddress() + 1) };
		StackPointer += 2;
		cycles--;
		return address;
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
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					Accumulator = ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::LDA_INDIRECT_Y:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
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
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					WriteByte(cycles, memory, effectiveAddress, Accumulator);
					break;
				}

				case Opcode::STA_INDIRECT_Y:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
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

				case Opcode::TAX:
				{
					XRegister = Accumulator;
					cycles--;
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::TAY:
				{
					YRegister = Accumulator;
					cycles--;
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::TXA:
				{
					Accumulator = XRegister;
					cycles--;
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::TYA:
				{
					Accumulator = YRegister;
					cycles--;
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_IMMEDIATE:
				{
					Accumulator &= FetchByte(cycles, memory);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Accumulator &= ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Accumulator &= ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Accumulator &= ReadByte(cycles, memory, absoluteAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, XRegister) };
					Accumulator &= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_ABSOLUTE_Y:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, YRegister) };
					Accumulator &= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_INDIRECT_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					Accumulator &= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::AND_INDIRECT_Y:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					Address finalAddress{ AddIndexed(cycles, effectiveAddress, YRegister) };
					Accumulator &= ReadByte(cycles, memory, finalAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_IMMEDIATE:
				{
					Accumulator ^= FetchByte(cycles, memory);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Accumulator ^= ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Accumulator ^= ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Accumulator ^= ReadByte(cycles, memory, absoluteAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, XRegister) };
					Accumulator ^= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_ABSOLUTE_Y:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, YRegister) };
					Accumulator ^= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_INDIRECT_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					Accumulator ^= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::EOR_INDIRECT_Y:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					Address finalAddress{ AddIndexed(cycles, effectiveAddress, YRegister) };
					Accumulator ^= ReadByte(cycles, memory, finalAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_IMMEDIATE:
				{
					Accumulator |= FetchByte(cycles, memory);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Accumulator |= ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Accumulator |= ReadByte(cycles, memory, zeroPageAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Accumulator |= ReadByte(cycles, memory, absoluteAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, XRegister) };
					Accumulator |= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_ABSOLUTE_Y:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ AddIndexed(cycles, absoluteAddress, YRegister) };
					Accumulator |= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_INDIRECT_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					cycles--;
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					Accumulator |= ReadByte(cycles, memory, effectiveAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::ORA_INDIRECT_Y:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, zeroPageAddress) };
					Address finalAddress{ AddIndexed(cycles, effectiveAddress, YRegister) };
					Accumulator |= ReadByte(cycles, memory, finalAddress);
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::BIT_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Byte value{ ReadByte(cycles, memory, zeroPageAddress) };
					SetFlag(Flag::Zero, (Accumulator & value) == 0);
					SetFlag(Flag::Negative, (value & Flag::Negative) != 0);
					SetFlag(Flag::Overflow, (value & Flag::Overflow) != 0);
					break;
				}

				case Opcode::BIT_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Byte value{ ReadByte(cycles, memory, absoluteAddress) };
					SetFlag(Flag::Zero, (Accumulator & value) == 0);
					SetFlag(Flag::Negative, (value & Flag::Negative) != 0);
					SetFlag(Flag::Overflow, (value & Flag::Overflow) != 0);
					break;
				}

				case Opcode::INC_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Byte value{ ReadByte(cycles, memory, zeroPageAddress) + 1u };
					cycles--;
					WriteByte(cycles, memory, zeroPageAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::INC_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					Byte value{ ReadByte(cycles, memory, zeroPageAddress) + 1u };
					cycles -= 2;
					WriteByte(cycles, memory, zeroPageAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::INC_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Byte value{ ReadByte(cycles, memory, absoluteAddress) + 1u };
					cycles--;
					WriteByte(cycles, memory, absoluteAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::INC_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ static_cast<Address>(absoluteAddress + XRegister) };
					Byte value{ ReadByte(cycles, memory, effectiveAddress) + 1u };
					cycles -= 2;
					WriteByte(cycles, memory, effectiveAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::INX:
				{
					XRegister++;
					cycles--;
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::INY:
				{
					YRegister++;
					cycles--;
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::DEC_ZERO_PAGE:
				{
					Byte zeroPageAddress{ FetchByte(cycles, memory) };
					Byte value{ ReadByte(cycles, memory, zeroPageAddress) - 1u };
					cycles--;
					WriteByte(cycles, memory, zeroPageAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::DEC_ZERO_PAGE_X:
				{
					Byte zeroPageAddress{ static_cast<Byte>(FetchByte(cycles, memory) + XRegister) };
					Byte value{ ReadByte(cycles, memory, zeroPageAddress) - 1u };
					cycles -= 2;
					WriteByte(cycles, memory, zeroPageAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::DEC_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Byte value{ ReadByte(cycles, memory, absoluteAddress) - 1u };
					cycles--;
					WriteByte(cycles, memory, absoluteAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::DEC_ABSOLUTE_X:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ static_cast<Address>(absoluteAddress + XRegister) };
					Byte value{ ReadByte(cycles, memory, effectiveAddress) - 1u };
					cycles -= 2;
					WriteByte(cycles, memory, effectiveAddress, value);
					LoadRegisterSetStatus(value);
					break;
				}

				case Opcode::DEX:
				{
					XRegister--;
					cycles--;
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::DEY:
				{
					YRegister--;
					cycles--;
					LoadRegisterSetStatus(YRegister);
					break;
				}

				case Opcode::TSX:
				{
					XRegister = StackPointer;
					cycles--;
					LoadRegisterSetStatus(XRegister);
					break;
				}

				case Opcode::TXS:
				{
					StackPointer = XRegister;
					cycles--;
					break;
				}

				case Opcode::PHA:
				{
					PushByteToStack(cycles, memory, Accumulator);
					cycles--;
					break;
				}

				case Opcode::PHP:
				{
					PushByteToStack(cycles, memory, ProcessorStatus | Flag::Break | Flag::Unused);
					cycles--;
					break;
				}

				case Opcode::PLA:
				{
					Accumulator = PopByteFromStack(cycles, memory);
					cycles--;
					LoadRegisterSetStatus(Accumulator);
					break;
				}

				case Opcode::PLP:
				{
					ProcessorStatus = static_cast<Byte>(PopByteFromStack(cycles, memory) & ~(Flag::Break | Flag::Unused));
					cycles--;
					break;
				}

				case Opcode::JMP_ABSOLUTE:
				{
					Address absoluteAddress{ FetchWord(cycles, memory) };
					ProgramCounter = absoluteAddress;
					break;
				}

				case Opcode::JMP_INDIRECT:
				{
					Address indirectAddress{ FetchWord(cycles, memory) };
					Address effectiveAddress{ ReadWordPageWrapped(cycles, memory, indirectAddress) };
					ProgramCounter = effectiveAddress;
					break;
				}

				case Opcode::JSR:
				{
					Address subroutineAddress{ FetchWord(cycles, memory) };
					PushProgramCounterToStack(cycles, memory);
					ProgramCounter = subroutineAddress;
					cycles--;
					break;
				}

				case Opcode::RTS:
				{
					ProgramCounter = PopAddressFromStack(cycles, memory) + 1;
					cycles -= 2;
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
