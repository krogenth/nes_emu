#include ".\include\CPU_Opcodes.h"

#include ".\include\Cartridge.h"
#include ".\include\PPU.h"
#include ".\include\APU.h"

#include ".\include\io_util.h"

CPUClass::CPUClass() {

	this->RAM.resize(0x2000);

	//	for debugging mode, open logfile and direct clog to it
#ifdef LOGGING
	this->logFile.open("log.txt", std::ios::out);
	std::clog.rdbuf(this->logFile.rdbuf());
#endif

}

CPUClass::~CPUClass() {

#ifdef LOGGING
	std::clog.rdbuf(this->clog_buf);
#endif

}

void CPUClass::power() {

	//	follow the nesdev guide on the power-up state: https://wiki.nesdev.com/w/index.php/CPU_power_up_state

	uint16_t addr = 0xFFFC;
	this->reg_PC = (uint16_t)this->access(addr) | ((uint16_t)this->access(addr + 1) << 8);

	//	reset registers
	this->reg_A = 0x00;
	this->reg_X = 0x00;
	this->reg_Y = 0x00;
	this->reg_FL = CPU_FLAGS::Unknown2 | CPU_FLAGS::Interrupt;
	this->reg_SP = 0xFD;

	//memset(this->RAM.data(), 0x00, this->RAM.size());

}

void CPUClass::reset() {

	//	see following for state of console after reset: https://wiki.nesdev.com/w/index.php/CPU_power_up_state

	this->reg_SP -= 3;
	this->setFlag(CPU_FLAGS::Interrupt);

}

void CPUClass::cycle() {

	//	here we can check the system type(NTSC vs PAL) and call the PPU an equivalent number of times
	this->cycleCount++;

}

void CPUClass::execute() {

#ifdef LOGGING
	std::clog << "CYC: " << std::setw(10) << this->cycleCount
			  << " PC: " << std::setw(10) << int_to_hex(this->reg_PC)
			  << " A: " << int_to_hex(this->reg_A)
			  << " X: " << int_to_hex(this->reg_X)
			  << " Y: " << int_to_hex(this->reg_Y)
			  << " FL: " << int_to_hex(this->reg_FL)
			  << " SP: " << int_to_hex(this->reg_SP)
			  << '\t';
#endif

	//	grab the next opcode and increment the Program Counter to retrieve any data
	switch (this->access(this->reg_PC++)) {

	case 0x00: this->BRK<addrMode_IMP>(); break;
	case 0x01: this->ORA<addrMode_INX>(); break;
	case 0x05: this->ORA<addrMode_ZP0>(); break;
	case 0x06: this->ASL<addrMode_ZP0>(); break;
	case 0x08: this->PHP<addrMode_IMP>(); break;
	case 0x09: this->ORA<addrMode_IMM>(); break;
	case 0x0A: this->ASL<addrMode_ACC>(); break;
	case 0x0D: this->ORA<addrMode_ABS>(); break;
	case 0x0E: this->ASL<addrMode_ABS>(); break;

	case 0x10: this->BPL<addrMode_REL>(); break;
	case 0x11: this->ORA<addrMode_INY>(); break;
	case 0x15: this->ORA<addrMode_ZPX>(); break;
	case 0x16: this->ASL<addrMode_ZPX>(); break;
	case 0x18: this->CLC<addrMode_IMP>(); break;
	case 0x19: this->ORA<addrMode_ABY>(); break;
	case 0x1D: this->ORA<addrMode_ABX>(); break;
	case 0x1E: this->ASL<addrMode_ABX>(); break;

	case 0x20: this->JSR<addrMode_ABS>(); break;
	case 0x21: this->AND<addrMode_INX>(); break;
	case 0x24: this->BIT<addrMode_ZP0>(); break;
	case 0x25: this->AND<addrMode_ZP0>(); break;
	case 0x26: this->ROL<addrMode_ZP0>(); break;
	case 0x28: this->PLP<addrMode_IMP>(); break;
	case 0x29: this->AND<addrMode_IMM>(); break;
	case 0x2A: this->ROL<addrMode_ACC>(); break;
	case 0x2C: this->BIT<addrMode_ABS>(); break;
	case 0x2D: this->AND<addrMode_ABS>(); break;
	case 0x2E: this->ROL<addrMode_ABS>(); break;

	case 0x30: this->BMI<addrMode_REL>(); break;
	case 0x31: this->AND<addrMode_INY>(); break;
	case 0x35: this->AND<addrMode_ZPX>(); break;
	case 0x36: this->ROL<addrMode_ZPX>(); break;
	case 0x38: this->SEC<addrMode_IMP>(); break;
	case 0x39: this->AND<addrMode_ABY>(); break;
	case 0x3D: this->AND<addrMode_ABX>(); break;
	case 0x3E: this->ROL<addrMode_ABX>(); break;

	case 0x40: this->RTI<addrMode_IMP>(); break;
	case 0x41: this->EOR<addrMode_INX>(); break;
	case 0x45: this->EOR<addrMode_ZP0>(); break;
	case 0x46: this->LSR<addrMode_ZP0>(); break;
	case 0x48: this->PHA<addrMode_IMP>(); break;
	case 0x49: this->EOR<addrMode_IMM>(); break;
	case 0x4A: this->LSR<addrMode_ACC>(); break;
	case 0x4C: this->JMP<addrMode_ABS>(); break;
	case 0x4D: this->EOR<addrMode_ABS>(); break;
	case 0x4E: this->LSR<addrMode_ABS>(); break;

	case 0x50: this->BVC<addrMode_REL>(); break;
	case 0x51: this->EOR<addrMode_INY>(); break;
	case 0x55: this->EOR<addrMode_ZPX>(); break;
	case 0x56: this->LSR<addrMode_ZPX>(); break;
	case 0x58: this->CLI<addrMode_IMP>(); break;
	case 0x59: this->EOR<addrMode_ABY>(); break;
	case 0x5D: this->EOR<addrMode_ABX>(); break;
	case 0x5E: this->LSR<addrMode_ABX>(); break;

	case 0x60: this->RTS<addrMode_IMP>(); break;
	case 0x61: this->ADC<addrMode_INX>(); break;
	case 0x65: this->ADC<addrMode_ZP0>(); break;
	case 0x66: this->ROR<addrMode_ZP0>(); break;
	case 0x68: this->PLA<addrMode_IMP>(); break;
	case 0x69: this->ADC<addrMode_IMM>(); break;
	case 0x6A: this->ROR<addrMode_ACC>(); break;
	case 0x6C: this->JMP<addrMode_IND>(); break;
	case 0x6D: this->ADC<addrMode_ABS>(); break;
	case 0x6E: this->ROR<addrMode_ABS>(); break;

	case 0x70: this->BVS<addrMode_REL>(); break;
	case 0x71: this->ADC<addrMode_INY>(); break;
	case 0x75: this->ADC<addrMode_ZPX>(); break;
	case 0x76: this->ROR<addrMode_ZPX>(); break;
	case 0x78: this->SEI<addrMode_IMP>(); break;
	case 0x79: this->ADC<addrMode_ABY>(); break;
	case 0x7D: this->ADC<addrMode_ABX>(); break;
	case 0x7E: this->ROR<addrMode_ABX>(); break;

	case 0x81: this->STA<addrMode_INX>(); break;
	case 0x84: this->STY<addrMode_ZP0>(); break;
	case 0x85: this->STA<addrMode_ZP0>(); break;
	case 0x86: this->STX<addrMode_ZP0>(); break;
	case 0x88: this->DEY<addrMode_IMP>(); break;
	case 0x8A: this->TXA<addrMode_IMP>(); break;
	case 0x8C: this->STY<addrMode_ABS>(); break;
	case 0x8D: this->STA<addrMode_ABS>(); break;
	case 0x8E: this->STX<addrMode_ABS>(); break;

	case 0x90: this->BCC<addrMode_REL>(); break;
	case 0x91: this->STA<addrMode_INY>(); break;
	case 0x94: this->STY<addrMode_ZPX>(); break;
	case 0x95: this->STA<addrMode_ZPX>(); break;
	case 0x96: this->STX<addrMode_ZPY>(); break;
	case 0x98: this->TYA<addrMode_IMP>(); break;
	case 0x99: this->STA<addrMode_ABY>(); break;
	case 0x9A: this->TXS<addrMode_IMP>(); break;
	case 0x9D: this->STA<addrMode_ABX>(); break;

	case 0xA0: this->LDY<addrMode_IMM>(); break;
	case 0xA1: this->LDA<addrMode_INX>(); break;
	case 0xA2: this->LDX<addrMode_IMM>(); break;
	case 0xA4: this->LDY<addrMode_ZP0>(); break;
	case 0xA5: this->LDA<addrMode_ZP0>(); break;
	case 0xA6: this->LDX<addrMode_ZP0>(); break;
	case 0xA8: this->TAY<addrMode_IMP>(); break;
	case 0xA9: this->LDA<addrMode_IMM>(); break;
	case 0xAA: this->TAX<addrMode_IMP>(); break;
	case 0xAC: this->LDY<addrMode_ABS>(); break;
	case 0xAD: this->LDA<addrMode_ABS>(); break;
	case 0xAE: this->LDX<addrMode_ABS>(); break;

	case 0xB0: this->BCS<addrMode_REL>(); break;
	case 0xB1: this->LDA<addrMode_INY>(); break;
	case 0xB4: this->LDY<addrMode_ZPX>(); break;
	case 0xB5: this->LDA<addrMode_ZPX>(); break;
	case 0xB6: this->LDX<addrMode_ZPY>(); break;
	case 0xB8: this->CLV<addrMode_IMP>(); break;
	case 0xB9: this->LDA<addrMode_ABY>(); break;
	case 0xBA: this->TSX<addrMode_IMP>(); break;
	case 0xBC: this->LDY<addrMode_ABX>(); break;
	case 0xBD: this->LDA<addrMode_ABX>(); break;
	case 0xBE: this->LDX<addrMode_ABY>(); break;

	case 0xC0: this->CPY<addrMode_IMM>(); break;
	case 0xC1: this->CMP<addrMode_INX>(); break;
	case 0xC4: this->CPY<addrMode_ZP0>(); break;
	case 0xC5: this->CMP<addrMode_ZP0>(); break;
	case 0xC6: this->DEC<addrMode_ZP0>(); break;
	case 0xC8: this->INY<addrMode_IMP>(); break;
	case 0xC9: this->CMP<addrMode_IMM>(); break;
	case 0xCA: this->DEX<addrMode_IMP>(); break;
	case 0xCC: this->CPY<addrMode_ABS>(); break;
	case 0xCD: this->CMP<addrMode_ABS>(); break;
	case 0xCE: this->DEC<addrMode_ABS>(); break;

	case 0xD0: this->BNE<addrMode_REL>(); break;
	case 0xD1: this->CMP<addrMode_INY>(); break;
	case 0xD5: this->CMP<addrMode_ZPX>(); break;
	case 0xD6: this->DEC<addrMode_ZPX>(); break;
	case 0xD8: this->CLD<addrMode_IMP>(); break;
	case 0xD9: this->CMP<addrMode_ABY>(); break;
	case 0xDD: this->CMP<addrMode_ABX>(); break;
	case 0xDE: this->DEC<addrMode_ABX>(); break;

	case 0xE0: this->CPX<addrMode_IMM>(); break;
	case 0xE1: this->SBC<addrMode_INX>(); break;
	case 0xE4: this->CPX<addrMode_ZP0>(); break;
	case 0xE5: this->SBC<addrMode_ZP0>(); break;
	case 0xE6: this->INC<addrMode_ZP0>(); break;
	case 0xE8: this->INX<addrMode_IMP>(); break;
	case 0xE9: this->SBC<addrMode_IMM>(); break;
	case 0xEA: this->NOP<addrMode_IMP>(); break;
	case 0xEC: this->CPX<addrMode_ABS>(); break;
	case 0xED: this->SBC<addrMode_ABS>(); break;
	case 0xEE: this->INC<addrMode_ABS>(); break;

	case 0xF0: this->BEQ<addrMode_REL>(); break;
	case 0xF1: this->SBC<addrMode_INY>(); break;
	case 0xF5: this->SBC<addrMode_ZPX>(); break;
	case 0xF6: this->INC<addrMode_ZPX>(); break;
	case 0xF8: this->SED<addrMode_IMP>(); break;
	case 0xF9: this->SBC<addrMode_ABY>(); break;
	case 0xFD: this->SBC<addrMode_ABX>(); break;
	case 0xFE: this->INC<addrMode_ABX>(); break;

	default: this->NOP<addrMode_IMP>(); break;

	}

#ifdef LOGGING
	std::clog << '\n' << std::flush;
#endif

}

uint8_t CPUClass::access(uint16_t address, uint8_t data, bool isWrite) {

	if (address <= 0x1FFF) {

		if (isWrite)
			return this->RAM.at(address % 0x800) = data;
		return this->RAM.at(address % 0x800);

	}
	else if (address <= 0x3FFF) {

		//	PPU access
		//return PPU->access(address, data, isWrite);
		return 0;

	}
	else if (address <= 0x4015) {

		//	APU access
		return 0;

	}
	else if (address == 0x4016) {

		//	APU access or joystick 1
		return 0;

	}
	else if (address == 0x4017) {

		//	APU access or joystick 2
		return 0;

	}
	else if (address <= 0x401F) {

		//	typically disabled APU/IO access
		return 0;

	}
	else
		return cartridge->prg_access(address, data, isWrite);

}