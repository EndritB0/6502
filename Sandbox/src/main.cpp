#include "pch.h"

#include <CPU.h>

int main()
{
	MOS6502::Memory memory;
	MOS6502::CPU cpu;
	cpu.Reset(memory);

	memory[0xFFFC] = MOS6502::Opcode::JSR;
	memory[0xFFFD] = 0x42;
	memory[0xFFFE] = 0x42;
	memory[0x4242] = MOS6502::Opcode::LDA_IMMEDIATE;
	memory[0x4243] = 0x67;

	cpu.Execute(9, memory);

	return 0;
}
