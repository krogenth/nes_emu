#include ".\CPU.h"
#include ".\io_util.h"

/*
For all timing work, please see: http://obelisk.me.uk/6502/reference.html
*/

/*
ADDRESSING MODES

For all accesses, we must increment the program counter as well
Every addressing mode returns an address for the opcodes to use

To better understand the addressing modes and how they function,
it is highly recommended to read the following links:
http://www.obelisk.me.uk/6502/addressing.html
http://www.emulator101.com/6502-addressing-modes.html
*/
uint16_t addrMode_IMM(CPUClass& CPU) {	//	0 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_IMM";
#endif

	//	the address of the next byte is the byte to read
	return CPU.reg_PC++;

}

uint16_t addrMode_IMP(CPUClass& CPU) {	//	0 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_IMP";
#endif

	/*
	we don't actually have to do anything, the Implied addressing mode
	essentially forces a specific place/value to be used,
	i.e.- CLC always clears the Carry flag.
	*/
	//	addrMode_IMP always costs 2 cycles
	CPU.cycle(); CPU.cycle();
	return 0;

}

uint16_t addrMode_ACC(CPUClass& CPU) {	//	0 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_ACC";
#endif

	/*
	this is essentially the same as the Implied addressing mode,
	however, the target is always the register A.
	*/
	return 0;

}

uint16_t addrMode_ABS(CPUClass& CPU) {	//	2 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_ABS";
#endif

	//	we access twice here, so we need to cycle twice
	CPU.cycle(); CPU.cycle();
	//	we use the next two bytes as the absolute address to use
	uint16_t addr = (uint16_t)CPU.access(CPU.reg_PC) | ((uint16_t)CPU.access(CPU.reg_PC + 1) << 8);
	CPU.reg_PC += 2;

	return addr;

}

uint16_t addrMode_ABX(CPUClass& CPU) {	//	2-3 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_ABX";
#endif

	//	this piggybacks off the addrMode_ABS, and adds the register X to it
	uint16_t addr = (uint16_t)addrMode_ABS(CPU);

	//	crossing page boundaries costs a cycle
	if((addr & 0xFF00) != ((addr + CPU.reg_X) & 0xFF00))
		CPU.cycle();

	addr += CPU.reg_X;
	return addr;

}

uint16_t addrMode_ABY(CPUClass& CPU) {	//	2-3 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_ABY";
#endif

	//	the same as addrMode_ABX, but with the register Y
	uint16_t addr = addrMode_ABS(CPU);

	//	crossing page boundaries costs a cycle
	if ((addr & 0xFF00) != ((addr + CPU.reg_Y) & 0xFF00))
		CPU.cycle();

	addr += CPU.reg_Y;
	return addr;

}

uint16_t addrMode_ZP0(CPUClass& CPU) {	//	1 cycle

#ifdef CPU_LOGGING
	CPU.logFile << "_ZP0";
#endif

	//	all accesses cost a cycle
	CPU.cycle();
	/*
	the next byte states the zero page(0x00FF, 256 bytes) address to use
	since addrMode_IMM gives us the location of the next byte, we can piggyback
	in order to quickly find the appropriate address
	*/
	uint16_t addr = (uint16_t)CPU.access(addrMode_IMM(CPU));
	return addr;

}

uint16_t addrMode_ZPX(CPUClass& CPU) {	//	2 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_ZPX";
#endif

	/*
	this piggybacks off the addrMode_ZP0, but adds the register X to the result
	any carry past 0x00FF will be dropped off, i.e.- 0xC0 + 0x60 = 0x120 -> 0x20
	*/
	uint16_t addr = addrMode_ZP0(CPU);

	//	we'll count the arithmetic as a cycle cost
	CPU.cycle();
	addr += CPU.reg_X;
	addr &= 0x00FF;
	return addr;

}

uint16_t addrMode_ZPY(CPUClass& CPU) {	//	2 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_ZPY";
#endif

	//	the same idea as addrMode_ZPX, but with register Y
	//	additionally, this mode can only be used with LDX and STX opcodes
	uint16_t addr = addrMode_ZP0(CPU);

	//	we'll count the arithmetic as a cycle cost
	CPU.cycle();
	addr += CPU.reg_Y;
	addr &= 0x00FF;
	return addr;

}

uint16_t addrMode_REL(CPUClass& CPU) {	//	2 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_REL";
#endif

	//	all branching opcodes cost a minimum of 2 cycles, we can do that here
	CPU.cycle(); CPU.cycle();
	uint16_t offset = (uint16_t)CPU.access(CPU.reg_PC);
	CPU.reg_PC++;
	//	check if the most significant bit of the read byte is 1
	//	if true, we need to expand it to be a full 16 bit negative
	if (offset & 0x0080)
		offset |= 0xFF00;

	return offset;

}

uint16_t addrMode_IND(CPUClass& CPU) {	//	4 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_IND";
#endif

	/*
	only supported by JMP opcode, as such, this will be a special case
	the register PC is to store the address stored at the address of the next two bytes to read,
	this will cause a jump in execution to the branch
	as such, this mode can piggyback off the addrMode_ABS to get the address
	the hardware bug mentioned by OLC will be reproduced inside the JMP opcode
	*/
	uint16_t addr = addrMode_ABS(CPU);

#ifdef CPU_LOGGING
	CPU.logFile << " " << int_to_hex(addr);
#endif

	//	we access twice here, so we need to cycle twice
	CPU.cycle(); CPU.cycle();
	//	the high byte access must wrap around to start of page(0xXX00) if addr ends in 0xXXFF
	if((addr & 0x00FF) == 0x00FF)
		return ((uint16_t)CPU.access(addr) | (uint16_t)(CPU.access((addr & 0xFF00)) << 8));
	else
		return ((uint16_t)CPU.access(addr) | (uint16_t)(CPU.access((addr + 1)) << 8));

}

uint16_t addrMode_INX(CPUClass& CPU) {	//	4 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_INX";
#endif

	/*
	piggybacks off of addrMode_ZPX to get the address within the zero page
	of the actual address we want to use
	see emulator101's "Indexed Indirect"
	*/
	uint16_t temp = addrMode_ZPX(CPU);

	//	we access twice here, so we need to cycle twice
	CPU.cycle(); CPU.cycle();
	//	we bitmask into the zero page again, in the chance that temp is actually 0x00FF
	uint16_t addr = (uint16_t)CPU.access(temp) | ((uint16_t)CPU.access((temp + 1) & 0x00FF) << 8);
	return addr;

}

uint16_t addrMode_INY(CPUClass& CPU) {	//	3-4 cycles

#ifdef CPU_LOGGING
	CPU.logFile << "_INY";
#endif

	/*
	instead of reading ($val + X) to find the correct address,
	we fine the address pointed to by ($val) and then add Y to that address
	with this method, we can piggyback off the addrMode_ZP0 to get the initial address
	*/
	uint16_t temp = addrMode_ZP0(CPU);

	//	we access twice here, so we need to cycle twice
	CPU.cycle(); CPU.cycle();
	//	we bitmask into the zero page again, in the chance that temp is actually 0x00FF
	uint16_t addr = (uint16_t)CPU.access(temp) | ((uint16_t)CPU.access((temp + 1) & 0x00FF) << 8);

	//	crossing page boundaries costs a cycle
	if ((addr & 0xFF00) != ((addr + CPU.reg_Y) & 0xFF00))
		CPU.cycle();

	//	based on ADC, INY can only cost 3-4 cycles to perform
	addr += CPU.reg_Y;
	return addr;

}


/*
OPCODES
*/

//	arithmetic
template<addrMode mode> void CPUClass::ADC() {

#ifdef CPU_LOGGING
	this->logFile << "ADC";
#endif

	//	all accesses cost a cycle
	this->cycle();
	//	get the data we need, and perform the calculation(always register A + data retrieved + carry flag)
	uint8_t data = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	arithmetic costs a cycle
	this->cycle();
	uint16_t result = this->reg_A + data + (uint8_t)this->getFlagState(CPU_FLAGS::Carry);

	this->updateCarry(result);
	this->updateZero(result);
	this->updateOverflow(result, (uint16_t)data);
	this->updateNegative(result);

	//	we are done updating flags, assign value to register A
	this->reg_A = (uint8_t)(result & 0x00FF);

}

template<addrMode mode> void CPUClass::SBC() {

#ifdef CPU_LOGGING
	this->logFile << "SBC";
#endif

	//	all accesses cost a cycle
	this->cycle();
	//	get the data we need, and perform the calculation(always register A + 255 - data + carry), 255 - data can be represented as 255 XOR data
	//	see the following links explaination of subtraction: https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
	uint8_t data = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	arithmetic costs a cycle
	this->cycle();
	uint16_t result = this->reg_A + (data ^ 0xFF) + (uint8_t)this->getFlagState(CPU_FLAGS::Carry);

	this->updateCarry(result);
	this->updateZero(result);
	this->updateOverflow(result, (uint16_t)(data ^ 0xFF));
	this->updateNegative(result);

	this->reg_A = (uint8_t)(result & 0x00FF);

}


//	bitwise
template<addrMode mode> void CPUClass::AND() {

#ifdef CPU_LOGGING
	this->logFile << "AND";
#endif

	//	all accesses cost a cycle, as does the bitwise operation
	this->cycle(); this->cycle();
	uint8_t data = this->access(mode(*this));
	this->reg_A &= data;

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	this->updateZero((uint16_t)this->reg_A);

	this->updateNegative((uint16_t)this->reg_A);

}

template<addrMode mode> void CPUClass::BIT() {

#ifdef CPU_LOGGING
	this->logFile << "BIT";
#endif

	//	all accesses cost a cycle
	this->cycle();
	//	we only test the bits, we do not store the AND result
	uint8_t data = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	bitwise operation costs a cycle
	this->cycle();
	this->updateZero((uint16_t)(this->reg_A & data));
	
	//	update Negative flag
	(data & 0b10000000) ? this->setFlag(CPU_FLAGS::Negative) : this->unsetFlag(CPU_FLAGS::Negative);

	//	update Overflow flag
	(data & 0b01000000) ? this->setFlag(CPU_FLAGS::Overflow) : this->unsetFlag(CPU_FLAGS::Overflow);

}

template<addrMode mode> void CPUClass::ORA() {

#ifdef CPU_LOGGING
	this->logFile << "ORA";
#endif

	//	all accesses cost a cycle, as does the bitwise operation
	this->cycle(); this->cycle();
	uint8_t data = this->access(mode(*this));
	this->reg_A |= data;

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	this->updateZero((uint16_t)this->reg_A);
	this->updateNegative((uint16_t)(this->reg_A & 0x80));

}

template<addrMode mode> void CPUClass::EOR() {

#ifdef CPU_LOGGING
	this->logFile << "EOR";
#endif

	//	all accesses cost a cycle, as does the bitwise operation
	this->cycle(); this->cycle();
	uint8_t data = this->access(mode(*this));
	this->reg_A ^= data;

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	this->updateZero((uint16_t)this->reg_A);
	this->updateNegative((uint16_t)(this->reg_A & 0x80));

}


//	bitwise shifts
template<addrMode mode> void CPUClass::ASL() {

#ifdef CPU_LOGGING
	this->logFile << "ASL";
#endif

	uint8_t data = 0;
	uint16_t addr = mode(*this);

	//	compare the function pointer to that of the addressing mode for the accumulator
	//	if it is the accumulator mode, store the register A into data to use, otherwise
	//	use the addressing mode given
	if (&mode == &addrMode_ACC)
		data = this->reg_A;
	else {

		//	all accesses cost a cycle
		this->cycle();
		data = this->access(addr);

	}

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	update Carry flag
	(data & 0x80) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry);

	//	bitwise operation costs a cycle
	this->cycle();
	//	perform the operation
	data <<= 1;

	//	always costs at minimum 2 cycles, so we need to call a second cycle here
	this->cycle();

	this->updateZero((uint16_t)data);
	this->updateNegative((uint16_t)data);

	//	finally, store the data
	if (&mode == &addrMode_ACC)
		this->reg_A = data;
	else {

		//	all accesses cost a cycle
		this->cycle();
		this->access(addr, data, true);

	}

}

template<addrMode mode> void CPUClass::LSR() {

#ifdef CPU_LOGGING
	this->logFile << "LSR";
#endif

	uint8_t data = 0;
	uint16_t addr = mode(*this);

	//	compare the function pointer to that of the addressing mode for the accumulator
	//	if it is the accumulator mode, store the register A into data to use, otherwise
	//	use the addressing mode given
	if (mode == &addrMode_ACC)
		data = this->reg_A;
	else {

		//	all accesses cost a cycle
		this->cycle();
		data = this->access(addr);

	}

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	update Carry flag
	(data & 0x01) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry);

	//	bitwise operation costs a cycle
	this->cycle();
	//	perform the operation
	data >>= 1;

	//	always costs at minimum 2 cycles, so we need to call a second cycle here
	this->cycle();

	this->updateZero((uint16_t)data);
	this->updateNegative((uint16_t)data);

	//	finally, store the data
	if (mode == &addrMode_ACC)
		this->reg_A = data;
	else {

		//	all accesses cost a cycle
		this->cycle();
		this->access(addr, data, true);

	}

}

template<addrMode mode> void CPUClass::ROL() {

#ifdef CPU_LOGGING
	this->logFile << "ROL";
#endif

	uint8_t data = 0;
	uint8_t carry = 0;
	uint16_t addr = mode(*this);

	//	compare the function pointer to that of the addressing mode for the accumulator
	//	if it is the accumulator mode, store the register A into data to use, otherwise
	//	use the addressing mode given
	if (mode == &addrMode_ACC)
		data = this->reg_A;
	else {

		//	all accesses cost a cycle
		this->cycle();
		data = this->access(addr);

	}

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	grab the current carry flag
	carry = (uint8_t)this->getFlagState(CPU_FLAGS::Carry);

	//	update Carry flag
	(data & 0x80) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry);

	//	bitwise operation costs a cycle
	this->cycle();
	//	perform the operation
	data = (data << 1) + carry;

	//	always costs at minimum 2 cycles, so we need to call a second cycle here
	this->cycle();

	this->updateZero((uint16_t)data);
	this->updateNegative((uint16_t)data);

	//	finally, store the data
	if (mode == &addrMode_ACC)
		this->reg_A = data;
	else {

		//	all accesses cost a cycle
		this->cycle();
		this->access(addr, data, true);

	}

}

template<addrMode mode> void CPUClass::ROR() {

#ifdef CPU_LOGGING
	this->logFile << "ROR";
#endif

	uint8_t data = 0;
	uint8_t carry = 0;
	uint16_t addr = mode(*this);

	//	compare the function pointer to that of the addressing mode for the accumulator
	//	if it is the accumulator mode, store the register A into data to use, otherwise
	//	use the addressing mode given
	if (mode == &addrMode_ACC)
		data = this->reg_A;
	else {

		//	all accesses cost a cycle
		this->cycle();
		data = this->access(addr);

	}

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	grab the current carry flag
	carry = (uint8_t)this->getFlagState(CPU_FLAGS::Carry);

	//	update Carry flag
	(data & 0x01) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry);

	//	bitwise operation costs a cycle
	this->cycle();
	//	perform the operation
	data = (data >> 1) + (carry << 7);

	//	always costs at minimum 2 cycles, so we need to call a second cycle here
	this->cycle();

	this->updateZero((uint16_t)data);
	this->updateNegative((uint16_t)data);

	//	finally, store the data
	if (mode == &addrMode_ACC)
		this->reg_A = data;
	else {

		//	all accesses cost a cycle
		this->cycle();
		this->access(addr, data, true);

	}

}


//	Decrement/Increment
template<addrMode mode> void CPUClass::DEC() {

#ifdef CPU_LOGGING
	this->logFile << "DEC";
#endif

	uint16_t addr = mode(*this);

	//	all accesses cost a cycle
	this->cycle();
	uint8_t data = this->access(addr);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	somehow this opcode takes minimum 5 cycles, therefore this decrement costs 2 cycles
	this->cycle(); this->cycle();
	data--;

	this->updateZero((uint16_t)data);
	this->updateNegative((uint16_t)data);

	//	all accesses cost a cycle
	this->cycle();
	this->access(addr, data, true);

}

template<addrMode mode> void CPUClass::DEX() {

#ifdef CPU_LOGGING
	this->logFile << "DEX";
#endif

	mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_X);
#endif

	this->reg_X--;

	this->updateZero((uint16_t)this->reg_X);
	this->updateNegative((uint16_t)this->reg_X);

}

template<addrMode mode> void CPUClass::DEY() {

#ifdef CPU_LOGGING
	this->logFile << "DEY";
#endif

	mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_Y);
#endif

	this->reg_Y--;

	this->updateZero((uint16_t)this->reg_Y);
	this->updateNegative((uint16_t)this->reg_Y);

}

template<addrMode mode> void CPUClass::INC() {

#ifdef CPU_LOGGING
	this->logFile << "INC";
#endif

	uint16_t addr = mode(*this);

	//	all accesses cost a cycle
	this->cycle();
	uint8_t data = this->access(addr);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	somehow this opcode takes minimum 5 cycles, therefore this decrement costs 2 cycles
	this->cycle(); this->cycle();
	data++;

	this->updateZero((uint16_t)data);
	this->updateNegative((uint16_t)data);

	//	all accesses cost a cycle
	this->cycle();
	this->access(addr, data, true);

}

template<addrMode mode> void CPUClass::INX() {

#ifdef CPU_LOGGING
	this->logFile << "INX";
#endif

	mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_X);
#endif

	this->reg_X++;

	this->updateZero((uint16_t)this->reg_X);
	this->updateNegative((uint16_t)this->reg_X);

}

template<addrMode mode> void CPUClass::INY() {

#ifdef CPU_LOGGING
	this->logFile << "INY";
#endif

	mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_Y);
#endif

	this->reg_Y++;

	this->updateZero((uint16_t)this->reg_Y);
	this->updateNegative((uint16_t)this->reg_Y);

}


//	jumps
template<addrMode mode> void CPUClass::JMP() {

#ifdef CPU_LOGGING
	this->logFile << "JMP";
#endif

	//	this opcode costs minimum 3 cycles, ABS by itself costs 2, so cycle once
	this->cycle();
	this->reg_PC = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_PC);
#endif

}

template<addrMode mode> void CPUClass::JSR() {

#ifdef CPU_LOGGING
	this->logFile << "JSR";
#endif

	//	store the next byte as return address for later

	this->push(((this->reg_PC + 1) & 0xFF00) >> 8);
	this->push(((this->reg_PC + 1) & 0x00FF));

	//	this opcode costs 6 cycles, and only uses addrMode_ABS
	//	ABS costs 2 cycles, and pushing reg_PC costs 2 cycles
	//	therefore, we need to cycle 2 more times
	this->cycle(); this->cycle();

	//	grab new Program Counter with whatever addressing mode we are using
	this->reg_PC = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_PC);
#endif

}

template<addrMode mode> void CPUClass::RTS() {

#ifdef CPU_LOGGING
	this->logFile << "RTS";
#endif

	//	this opcode costs 6 cycles, and only uses addrMode_IMP
	//	popping the reg_PC costs 2 cycles, therefore we cycle 4 more times
	mode(*this); this->cycle(); this->cycle();
	this->reg_PC = (this->pop() | (this->pop() << 8)) + 1;

}

template<addrMode mode> void CPUClass::RTI() {

#ifdef CPU_LOGGING
	this->logFile << "RTI";
#endif

	this->PLP<mode>();
	this->reg_PC = this->pop() | (this->pop() << 8);

}


//	loads
template<addrMode mode> void CPUClass::LDA() {

#ifdef CPU_LOGGING
	this->logFile << "LDA";
#endif

	//	all accesses cost a cycle
	this->cycle();
	this->reg_A = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_A);
#endif

	//	opcode costs minimum 2 cycles, so we need to cycle once more
	this->cycle();

	this->updateZero((uint16_t)this->reg_A);
	this->updateNegative((uint16_t)this->reg_A);

}

template<addrMode mode> void CPUClass::LDX() {

#ifdef CPU_LOGGING
	this->logFile << "LDX";
#endif

	//	all accesses cost a cycle
	this->cycle();
	this->reg_X = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_X);
#endif

	//	opcode costs minimum 2 cycles, so we need to cycle once more
	this->cycle();

	this->updateZero((uint16_t)this->reg_X);
	this->updateNegative((uint16_t)this->reg_X);

}

template<addrMode mode> void CPUClass::LDY() {

#ifdef CPU_LOGGING
	this->logFile << "LDY";
#endif

	//	all accesses cost a cycle
	this->cycle();
	this->reg_Y = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_Y);
#endif

	//	opcode costs minimum 2 cycles, so we need to cycle once more
	this->cycle();

	this->updateZero((uint16_t)this->reg_Y);
	this->updateNegative((uint16_t)this->reg_Y);

}


//	no operation
template<addrMode mode> void CPUClass::NOP() {

#ifdef CPU_LOGGING
	this->logFile << "NOP";
#endif

	mode(*this);

}

//	stack operations
template<addrMode mode> void CPUClass::PHA() {

#ifdef CPU_LOGGING
	this->logFile << "PHA";
#endif

	//	opcode costs 3 cycles total, push costs 1, so cycle 2 more times
	mode(*this);
	this->push(this->reg_A);

}

template<addrMode mode> void CPUClass::PLA() {

#ifdef CPU_LOGGING
	this->logFile << "PLA";
#endif

	//	opcode costs 4 cycles total, pop costs 1, so cycle 3 more times
	mode(*this); this->cycle();
	this->reg_A = this->pop();

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_A);
#endif

	this->updateZero((uint16_t)this->reg_A);
	this->updateNegative((uint16_t)this->reg_A);

}

template<addrMode mode> void CPUClass::PHP() {

#ifdef CPU_LOGGING
	this->logFile << "PHP";
#endif

	//	opcode costs 3 cycles total, push costs 1, so cycle 2 more times
	mode(*this);
	this->push(this->reg_FL);

}

template<addrMode mode> void CPUClass::PLP() {

#ifdef CPU_LOGGING
	this->logFile << "PLP";
#endif

	//	this opcode always costs 4 cycles, pop costs 1, so cycle 3 more times
	mode(*this); this->cycle();
	this->reg_FL = this->pop();

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_FL);
#endif

}


//	register storing
template<addrMode mode> void CPUClass::TAX() {

#ifdef CPU_LOGGING
	this->logFile << "TAX";
#endif

	mode(*this);
	this->reg_X = this->reg_A;

	this->updateZero((uint16_t)this->reg_X);
	this->updateNegative((uint16_t)this->reg_X);

}

template<addrMode mode> void CPUClass::TAY() {

#ifdef CPU_LOGGING
	this->logFile << "TAY";
#endif

	mode(*this);
	this->reg_Y = this->reg_A;
	this->updateZero((uint16_t)this->reg_Y);
	this->updateNegative((uint16_t)this->reg_Y);


}

template<addrMode mode> void CPUClass::TSX() {

#ifdef CPU_LOGGING
	this->logFile << "TSX";
#endif

	mode(*this);
	this->reg_X = this->reg_SP;

	this->updateZero((uint16_t)this->reg_X);
	this->updateNegative((uint16_t)this->reg_X);

}

template<addrMode mode> void CPUClass::TXA() {

#ifdef CPU_LOGGING
	this->logFile << "TXA";
#endif

	mode(*this);
	this->reg_A = this->reg_X;

	this->updateZero((uint16_t)this->reg_A);
	this->updateNegative((uint16_t)this->reg_A);

}

template<addrMode mode> void CPUClass::TYA() {

#ifdef CPU_LOGGING
	this->logFile << "TYA";
#endif

	mode(*this);
	this->reg_A = this->reg_Y;

	this->updateZero((uint16_t)this->reg_A);
	this->updateNegative((uint16_t)this->reg_A);

}

template<addrMode mode> void CPUClass::TXS() {

#ifdef CPU_LOGGING
	this->logFile << "TSX";
#endif

	mode(*this);
	this->reg_SP = this->reg_X;

}


//	memory storing
template<addrMode mode> void CPUClass::STA() {

#ifdef CPU_LOGGING
	this->logFile << "STA";
#endif

	//	store register A at address
	uint16_t addr = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(addr);
#endif

	//	opcode costs minimum 3 cycles with addrMode_ZP0(1 cycle), so cycle 2 more times
	this->cycle(); this->cycle();
	this->access(addr, this->reg_A, true);

}

template<addrMode mode> void CPUClass::STX() {

#ifdef CPU_LOGGING
	this->logFile << "STX";
#endif

	//	store register X at address
	uint16_t addr = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(addr);
#endif

	//	opcode costs minimum 3 cycles with addrMode_ZP0(1 cycle), so cycle 2 more times
	this->cycle(); this->cycle();
	this->access(addr, this->reg_X, true);

}

template<addrMode mode> void CPUClass::STY() {

#ifdef CPU_LOGGING
	this->logFile << "STY";
#endif

	//	store register Y at address
	uint16_t addr = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(addr);
#endif

	//	opcode costs minimum 3 cycles with addrMode_ZP0(1 cycle), so cycle 2 more times
	this->cycle(); this->cycle();
	this->access(addr, this->reg_Y, true);

}


//	branching
template<addrMode mode> void CPUClass::BCC() {

#ifdef CPU_LOGGING
	this->logFile << "BCC";
#endif

	//	branch only if the last calculation resulted in no carry

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	//uint8_t offset = this->access(mode(*this));
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Carry) == 0) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}

template<addrMode mode> void CPUClass::BCS() {

#ifdef CPU_LOGGING
	this->logFile << "BCS";
#endif

	//	branch only if the last calculation resulted in a carry

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Carry) == true) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}

template<addrMode mode> void CPUClass::BEQ() {

#ifdef CPU_LOGGING
	this->logFile << "BEQ";
#endif

	//	branch only if the last calcuation resulted in a zero

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Zero) == true) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}

template<addrMode mode> void CPUClass::BMI() {

#ifdef CPU_LOGGING
	this->logFile << "BMI";
#endif

	//	branch only if the last calcuation resulted in a negative

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Negative) == true) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}

template<addrMode mode> void CPUClass::BNE() {

#ifdef CPU_LOGGING
	this->logFile << "BNE";
#endif

	//	branch only if the last calcuation resulted in a non-zero

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Zero) == false) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}

template<addrMode mode> void CPUClass::BPL() {

#ifdef CPU_LOGGING
	this->logFile << "BPL";
#endif

	//	branch only if the last calcuation resulted in a positive

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Negative) == false) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}

template<addrMode mode> void CPUClass::BVC() {

#ifdef CPU_LOGGING
	this->logFile << "BVC";
#endif

	//	branch only if the last calcuation resulted in no overflow

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Overflow) == false) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}

template<addrMode mode> void CPUClass::BVS() {

#ifdef CPU_LOGGING
	this->logFile << "BVS";
#endif

	//	branch only if the last calcuation resulted in overflow

	//	we still need to read the data related to the opcode, to go voer the entire opcode
	uint16_t offset = mode(*this);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(offset);
#endif

	if (this->getFlagState(CPU_FLAGS::Overflow) == true) {

		//	we cycle just for branching
		this->cycle();

		//	if a page boundary is crossed, we cycle again
		if ((reg_PC & 0xFF00) != ((reg_PC + offset) & 0xFF00))
			this->cycle();

		this->reg_PC += offset;

	}

}


//	flag operations
template<addrMode mode> void CPUClass::CLC() {

#ifdef CPU_LOGGING
	this->logFile << "CLC";
#endif

	mode(*this);
	this->unsetFlag(CPU_FLAGS::Carry);

}

template<addrMode mode> void CPUClass::CLD() {

#ifdef CPU_LOGGING
	this->logFile << "CLD";
#endif

	mode(*this);
	this->unsetFlag(CPU_FLAGS::Decimal);

}

template<addrMode mode> void CPUClass::CLI() {

#ifdef CPU_LOGGING
	this->logFile << "CLI";
#endif

	mode(*this);
	this->unsetFlag(CPU_FLAGS::Interrupt);

}

template<addrMode mode> void CPUClass::CLV() {

#ifdef CPU_LOGGING
	this->logFile << "CLV";
#endif

	mode(*this);
	this->unsetFlag(CPU_FLAGS::Overflow);

}

template<addrMode mode> void CPUClass::SEC() {

#ifdef CPU_LOGGING
	this->logFile << "SEC";
#endif

	mode(*this);
	this->setFlag(CPU_FLAGS::Carry);

}

template<addrMode mode> void CPUClass::SED() {

#ifdef CPU_LOGGING
	this->logFile << "SED";
#endif

	mode(*this);
	this->setFlag(CPU_FLAGS::Decimal);

}

template<addrMode mode> void CPUClass::SEI() {

#ifdef CPU_LOGGING
	this->logFile << "SEI";
#endif

	mode(*this);
	this->setFlag(CPU_FLAGS::Interrupt);

}


//	comparisons
template<addrMode mode> void CPUClass::CMP() {

#ifdef CPU_LOGGING
	this->logFile << "CMP";
#endif

	//	all accesses cost a cycle
	this->cycle();
	//	we do not store the result of this operation
	uint8_t data = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	arithmetic costs a cycle
	this->cycle();
	uint8_t result = this->reg_A - data;

	//	update Carry flag
	(this->reg_A >= data) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry);

	this->updateZero((uint16_t)result);
	this->updateNegative((uint16_t)result);

}

template<addrMode mode> void CPUClass::CPX() {

#ifdef CPU_LOGGING
	this->logFile << "CPX";
#endif

	//	all accesses cost a cycle
	this->cycle();
	//	we do not store the result of this operation
	uint8_t data = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	arithmetic costs a cycle
	this->cycle();
	uint8_t result = this->reg_X - data;

	//	update Carry flag
	(this->reg_X >= data) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry);

	this->updateZero((uint16_t)result);
	this->updateNegative((uint16_t)result);

}

template<addrMode mode> void CPUClass::CPY() {

#ifdef CPU_LOGGING
	this->logFile << "CPY";
#endif

	//	all accesses cost a cycle
	this->cycle();
	//	we do not store the result of this operation
	uint8_t data = this->access(mode(*this));

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(data);
#endif

	//	arithmetic costs a cycle
	this->cycle();
	uint8_t result = this->reg_Y - data;

	//	update Carry flag
	(this->reg_Y >= data) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry);

	this->updateZero((uint16_t)result);
	this->updateNegative((uint16_t)result);

}


//	break
template<addrMode mode> void CPUClass::BRK() {

#ifdef CPU_LOGGING
	this->logFile << "BRK";
#endif

	//	needs to perform an interrupt here
	//	set Interrupt flag
	this->SEI<mode>();

	//	push the register Program Counter onto the stack
	this->push((uint8_t)((this->reg_PC & 0xFF00) >> 8));
	this->push((uint8_t)(this->reg_PC & 0x00FF));

	//	for some reason, the BRK instruction sets the pushed status flags bits
	//	5 and 6 to 1, so we can replicate that here
	//	see the following for why: https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
	this->setFlag(CPU_FLAGS::B_flag);
	this->setFlag(CPU_FLAGS::Unknown2);

	this->push(this->reg_FL);

	//	since it is only the pushed status flag that has the bits set, we now unset them
	this->unsetFlag(CPU_FLAGS::B_flag);
	this->unsetFlag(CPU_FLAGS::Unknown2);

	mode(*this);
	//	and we change the Program Counter to be at whatever FFFE points to
	this->reg_PC = (uint16_t)this->access(0xFFFE) | ((uint16_t)this->access(0xFFFF) << 8);

#ifdef CPU_LOGGING
	this->logFile << " " << int_to_hex(this->reg_PC);
#endif

}



/*
now define all the template types of the opcodes
146 unique opcodes
*/

//	define all BRK types
template void CPUClass::BRK<addrMode_IMP>();

//	define all ORA types
template void CPUClass::ORA<addrMode_INX>();
template void CPUClass::ORA<addrMode_ZP0>();
template void CPUClass::ORA<addrMode_IMM>();
template void CPUClass::ORA<addrMode_ABS>();
template void CPUClass::ORA<addrMode_INY>();
template void CPUClass::ORA<addrMode_ZPX>();
template void CPUClass::ORA<addrMode_ABY>();
template void CPUClass::ORA<addrMode_ABX>();

//	define all ASL types
template void CPUClass::ASL<addrMode_ZP0>();
template void CPUClass::ASL<addrMode_ACC>();
template void CPUClass::ASL<addrMode_ABS>();
template void CPUClass::ASL<addrMode_ZPX>();
template void CPUClass::ASL<addrMode_ABX>();

//	define all PHP types
template void CPUClass::PHP<addrMode_IMP>();

//	define all BPL types
template void CPUClass::BPL<addrMode_REL>();

//	define all CLC types
template void CPUClass::CLC<addrMode_IMP>();

//	define all JSR types
template void CPUClass::JSR<addrMode_ABS>();

//	define all AND types
template void CPUClass::AND<addrMode_INX>();
template void CPUClass::AND<addrMode_ZP0>();
template void CPUClass::AND<addrMode_IMM>();
template void CPUClass::AND<addrMode_ABS>();
template void CPUClass::AND<addrMode_INY>();
template void CPUClass::AND<addrMode_ZPX>();
template void CPUClass::AND<addrMode_ABY>();
template void CPUClass::AND<addrMode_ABX>();

//	define all BIT types
template void CPUClass::BIT<addrMode_ZP0>();
template void CPUClass::BIT<addrMode_ABS>();

//	define all ROL types
template void CPUClass::ROL<addrMode_ZP0>();
template void CPUClass::ROL<addrMode_ACC>();
template void CPUClass::ROL<addrMode_ABS>();
template void CPUClass::ROL<addrMode_ZPX>();
template void CPUClass::ROL<addrMode_ABX>();

//	define all PLP types
template void CPUClass::PLP<addrMode_IMP>();

//	define all BMI types
template void CPUClass::BMI<addrMode_REL>();

//	define all SEC types
template void CPUClass::SEC<addrMode_IMP>();

//	define all RTI types
template void CPUClass::RTI<addrMode_IMP>();

//	define all EOR types
template void CPUClass::EOR<addrMode_INX>();
template void CPUClass::EOR<addrMode_ZP0>();
template void CPUClass::EOR<addrMode_IMM>();
template void CPUClass::EOR<addrMode_ABS>();
template void CPUClass::EOR<addrMode_INY>();
template void CPUClass::EOR<addrMode_ZPX>();
template void CPUClass::EOR<addrMode_ABY>();
template void CPUClass::EOR<addrMode_ABX>();

//	define all LSR types
template void CPUClass::LSR<addrMode_ZP0>();
template void CPUClass::LSR<addrMode_ACC>();
template void CPUClass::LSR<addrMode_ABS>();
template void CPUClass::LSR<addrMode_ZPX>();
template void CPUClass::LSR<addrMode_ABX>();

//	define all PHA types
template void CPUClass::PHA<addrMode_IMP>();

//	define all JMP types
template void CPUClass::JMP<addrMode_ABS>();
template void CPUClass::JMP<addrMode_IND>();

//	define all BVC types
template void CPUClass::BVC<addrMode_REL>();

//	define all CLI types
template void CPUClass::CLI<addrMode_IMP>();

//	define all RTS types
template void CPUClass::RTS<addrMode_IMP>();

//	define all ADC types
template void CPUClass::ADC<addrMode_INX>();
template void CPUClass::ADC<addrMode_ZP0>();
template void CPUClass::ADC<addrMode_IMM>();
template void CPUClass::ADC<addrMode_ABS>();
template void CPUClass::ADC<addrMode_INY>();
template void CPUClass::ADC<addrMode_ZPX>();
template void CPUClass::ADC<addrMode_ABY>();
template void CPUClass::ADC<addrMode_ABX>();

//	define all ROR types
template void CPUClass::ROR<addrMode_ZP0>();
template void CPUClass::ROR<addrMode_ACC>();
template void CPUClass::ROR<addrMode_ABS>();
template void CPUClass::ROR<addrMode_ZPX>();
template void CPUClass::ROR<addrMode_ABX>();

//	define all PLA types
template void CPUClass::PLA<addrMode_IMP>();

//	define all BVS types
template void CPUClass::BVS<addrMode_REL>();

//	define all SEI types
template void CPUClass::SEI<addrMode_IMP>();

//	define all STA types
template void CPUClass::STA<addrMode_INX>();
template void CPUClass::STA<addrMode_ZP0>();
template void CPUClass::STA<addrMode_ABS>();
template void CPUClass::STA<addrMode_INY>();
template void CPUClass::STA<addrMode_ZPX>();
template void CPUClass::STA<addrMode_ABY>();
template void CPUClass::STA<addrMode_ABX>();

//	define all STY types
template void CPUClass::STY<addrMode_ZP0>();
template void CPUClass::STY<addrMode_ABS>();
template void CPUClass::STY<addrMode_ZPX>();

//	define all STX types
template void CPUClass::STX<addrMode_ZP0>();
template void CPUClass::STX<addrMode_ABS>();
template void CPUClass::STX<addrMode_ZPY>();

//	define all DEY types
template void CPUClass::DEY<addrMode_IMP>();

//	define all TXA types
template void CPUClass::TXA<addrMode_IMP>();

//	define all BCC types
template void CPUClass::BCC<addrMode_REL>();

//	define all TYA types
template void CPUClass::TYA<addrMode_IMP>();

//	define all TXS types
template void CPUClass::TXS<addrMode_IMP>();

//	define all LDY types
template void CPUClass::LDY<addrMode_IMM>();
template void CPUClass::LDY<addrMode_ZP0>();
template void CPUClass::LDY<addrMode_ABS>();
template void CPUClass::LDY<addrMode_ZPX>();
template void CPUClass::LDY<addrMode_ABX>();

//	define all LDA types
template void CPUClass::LDA<addrMode_INX>();
template void CPUClass::LDA<addrMode_ZP0>();
template void CPUClass::LDA<addrMode_IMM>();
template void CPUClass::LDA<addrMode_ABS>();
template void CPUClass::LDA<addrMode_INY>();
template void CPUClass::LDA<addrMode_ZPX>();
template void CPUClass::LDA<addrMode_ABY>();
template void CPUClass::LDA<addrMode_ABX>();

//	define all LDX types
template void CPUClass::LDX<addrMode_IMM>();
template void CPUClass::LDX<addrMode_ZP0>();
template void CPUClass::LDX<addrMode_ABS>();
template void CPUClass::LDX<addrMode_ZPY>();
template void CPUClass::LDX<addrMode_ABY>();

//	define all CPY types
template void CPUClass::CPY<addrMode_IMM>();
template void CPUClass::CPY<addrMode_ZP0>();
template void CPUClass::CPY<addrMode_ABS>();

//	define all CMP types
template void CPUClass::CMP<addrMode_INX>();
template void CPUClass::CMP<addrMode_ZP0>();
template void CPUClass::CMP<addrMode_IMM>();
template void CPUClass::CMP<addrMode_ABS>();
template void CPUClass::CMP<addrMode_INY>();
template void CPUClass::CMP<addrMode_ZPX>();
template void CPUClass::CMP<addrMode_ABY>();
template void CPUClass::CMP<addrMode_ABX>();

//	define all DEC types
template void CPUClass::DEC<addrMode_ZP0>();
template void CPUClass::DEC<addrMode_ABS>();
template void CPUClass::DEC<addrMode_ZPX>();
template void CPUClass::DEC<addrMode_ABX>();

//	define all INY types
template void CPUClass::INY<addrMode_IMP>();

//	define all DEX types
template void CPUClass::DEX<addrMode_IMP>();

//	define all BNE types
template void CPUClass::BNE<addrMode_REL>();

//	define all CLD types
template void CPUClass::CLD<addrMode_IMP>();

//	define all CPX types
template void CPUClass::DEC<addrMode_IMM>();
template void CPUClass::DEC<addrMode_ZP0>();
template void CPUClass::DEC<addrMode_ABS>();

//	define all SBC types
template void CPUClass::SBC<addrMode_INX>();
template void CPUClass::SBC<addrMode_ZP0>();
template void CPUClass::SBC<addrMode_IMM>();
template void CPUClass::SBC<addrMode_ABS>();
template void CPUClass::SBC<addrMode_INY>();
template void CPUClass::SBC<addrMode_ZPX>();
template void CPUClass::SBC<addrMode_ABY>();
template void CPUClass::SBC<addrMode_ABX>();

//	define all INC types
template void CPUClass::INC<addrMode_ZP0>();
template void CPUClass::INC<addrMode_ABS>();
template void CPUClass::INC<addrMode_ZPX>();
template void CPUClass::INC<addrMode_ABX>();

//	define all INX types
template void CPUClass::INX<addrMode_IMP>();

//	define all NOP types
template void CPUClass::NOP<addrMode_IMP>();

//	define all BEQ types
template void CPUClass::BEQ<addrMode_REL>();

//	define all SED types
template void CPUClass::SED<addrMode_IMP>();