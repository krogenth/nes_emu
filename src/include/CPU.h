#ifndef CPU_H
#define CPU_H

#include <cinttypes>	//	(u)intx_t
#include <vector>		//	std::vector

#if defined (DEBUG) | defined(_DEBUG)
#define LOGGING
#include <fstream>		//	std::ifstream, std::ofstream
#include <iomanip>		//	std::setw
#include <iostream>		//	std::cout, std::cerr, std::clog
#endif

//	the entire register status flags is 8 bits, therefore we can save these values as 8 bit values
enum CPU_FLAGS : uint8_t {

	Carry     = 0b00000001,
	Zero      = 0b00000010,
	Interrupt = 0b00000100,
	Decimal   = 0b00001000,
	B_flag    = 0b00010000,
	Unknown2  = 0b00100000,
	Overflow  = 0b01000000,
	Negative  = 0b10000000

};

/*
we need to forward delcare the CPUClass here in order to define the addrMode function pointer.
with the function pointer, we can effectively pass in the addressing mdoes to the instructions
reducing significantly the amount of work required to code all opcodes
*/
class CPUClass;
typedef uint16_t(*addrMode)(CPUClass&);
class CartridgeClass;
class PPUClass;
class APUClass;

class CPUClass {

public:
	CPUClass();
	~CPUClass();

	//	used to load various components into CPU
	void loadCartridge(CartridgeClass* _cartridge) { cartridge = _cartridge; }
	void loadPPU(PPUClass* _PPU) { PPU = _PPU; }
	void loadAPU(APUClass* _APU) { APU = _APU; }

	//	used to "turn on" the console and reset console
	void power();
	void reset();

	//	used to determine what opcode to execute
	void execute();

protected:
	//	registers
	uint8_t reg_A = 0, reg_X = 0, reg_Y = 0, reg_FL = 0, reg_SP = 0xFF;
	uint16_t reg_PC = 0;
	uint32_t cycleCount = 0;

	std::vector<uint8_t> RAM;

	//	store pointers to various compnents for calls
	CartridgeClass* cartridge = nullptr;
	PPUClass* PPU = nullptr;
	APUClass* APU = nullptr;

	//	used in debug mode for logging
#ifdef LOGGING
	std::ofstream logFile;
	std::streambuf* clog_buf = std::clog.rdbuf();
#endif

	//	to handle the timing of instructions and PPU(PPU clock is 3x CPU clock(at least with NTSC, PAL is 2.4x))
	void cycle();

	//	declare addressing modes as friends to this class
	friend uint16_t addrMode_IMM(CPUClass& CPU);		//	immediate mode:           1 byte, data to use is next byte
	friend uint16_t addrMode_IMP(CPUClass& CPU);		//	implied mode:             0 bytes, data is already with opcode, look at opcode PHA for why we might want this
	friend uint16_t addrMode_ACC(CPUClass& CPU);		//	accumulator mode:         0 bytes, instruction targets the accumulator register, so this mode is unneeded
	friend uint16_t addrMode_ABS(CPUClass& CPU);		//	absolute mode:            2 bytes, next two bytes specify the memory address
	friend uint16_t addrMode_ABX(CPUClass& CPU);		//	absolute X-indexed mode:  2 bytes
	friend uint16_t addrMode_ABY(CPUClass& CPU);		//	absolute Y-indexed mode:  2 bytes
	friend uint16_t addrMode_ZP0(CPUClass& CPU);		//	zero-page mode:           1 byte, address is in zero-page(0x00FF)
	friend uint16_t addrMode_ZPX(CPUClass& CPU);		//	zero-page x-indexed mode: 1 byte, address + X is in zero-page(0x00FF)
	friend uint16_t addrMode_ZPY(CPUClass& CPU);		//	zero-page y-indexed mode: 1 byte, address + Y is in zero-page(0x00FF)
	friend uint16_t addrMode_REL(CPUClass& CPU);		//	relative mode:            1 byte
	friend uint16_t addrMode_IND(CPUClass& CPU);		//	indirect mode:            2 bytes
	friend uint16_t addrMode_INX(CPUClass& CPU);		//	indirect x-indexed mode:  1 byte, ($val + reg_X) -> $address
	friend uint16_t addrMode_INY(CPUClass& CPU);		//	indirect y-indexed mode:  1 byte, ($val) + reg_Y -> $address

	//	to access the CPU stack address space(this is the second page of the CPU RAM space, 0x0100 to 0x01FF)
	inline void push(uint8_t data) { this->cycle(); this->access(0x0100 + (this->reg_SP--), data, true); }
	inline uint8_t pop() { this->cycle(); return this->access(0x0100 + (++this->reg_SP)); }

	//	used to read/write data from the various components(APU, PPU, Cartridge, Peripheral)
	uint8_t access(uint16_t address, uint8_t data = 0, bool isWrite = false);

	//	helper functions
	inline void setFlag(CPU_FLAGS flag) { this->reg_FL |= flag; }
	inline void unsetFlag(CPU_FLAGS flag) { this->reg_FL &= ~(flag); }
	inline bool getFlagState(CPU_FLAGS flag) { return ((this->reg_FL & flag) == flag); }

	inline void updateCarry(uint16_t result) { (result & 0x0100) ? this->setFlag(CPU_FLAGS::Carry) : this->unsetFlag(CPU_FLAGS::Carry); }
	inline void updateZero(uint16_t result) { ((result & 0x00FF) == 0x0000) ? this->setFlag(CPU_FLAGS::Zero) : this->unsetFlag(CPU_FLAGS::Zero); }
	inline void updateOverflow(uint16_t result, uint16_t read) { (((uint16_t)this->reg_A ^ result) & ((uint16_t)read ^ result) & 0x0080) ? this->setFlag(CPU_FLAGS::Overflow) : this->unsetFlag(CPU_FLAGS::Overflow); }	//	see the following link on why this works: http://forums.nesdev.com/viewtopic.php?t=6331
	inline void updateNegative(uint16_t result) { (result & 0x0080) ? this->setFlag(CPU_FLAGS::Negative) : this->unsetFlag(CPU_FLAGS::Negative); }

	//	arithmetic
	template<addrMode mode> void ADC();		//	reg_A + b + carry
	template<addrMode mode> void SBC();		//	reg_A - b + c

	//	bitwise
	template<addrMode mode> void AND();		//	reg_A & b
	template<addrMode mode> void BIT();		//	zero = reg_A & b, negative = b & 0b10000000, overflow = b & 0b01000000
	template<addrMode mode> void ORA();		//	reg_A | b
	template<addrMode mode> void EOR();		//	reg_A ^ b

	//	bitwise shifts
	template<addrMode mode> void ASL();		//	a << 1
	template<addrMode mode> void LSR();		//	a >> 1
	template<addrMode mode> void ROL();		//	(a << 1) | carry, carry = (a & 0b10000000)
	template<addrMode mode> void ROR();		//	(a >> 1) | carry << 7, carry =(a & 0b00000001)

	//	Decrement/Increment
	template<addrMode mode> void DEC();		//	a--
	template<addrMode mode> void DEX();		//	reg_X--
	template<addrMode mode> void DEY();		//	reg_Y--
	template<addrMode mode> void INC();		//	b++
	template<addrMode mode> void INX();		//	reg_X++
	template<addrMode mode> void INY();		//	reg_Y++

	//	jumps
	template<addrMode mode> void JMP();		//	jump to new location
	template<addrMode mode> void JSR();		//	jump to new location, push current reg_PC onto stack
	template<addrMode mode> void RTS();		//	return from subroutine
	template<addrMode mode> void RTI();		//	return from interrupt

	//	loads
	template<addrMode mode> void LDA();		//	reg_A = b
	template<addrMode mode> void LDX();		//	reg_X = b
	template<addrMode mode> void LDY();		//	reg_Y = b

	//	no operation
	template<addrMode mode> void NOP();

	//	stack operations
	template<addrMode mode> void PHA();		//	push reg_A onto stack
	template<addrMode mode> void PLA();		//	pull reg_A from stack
	template<addrMode mode> void PHP();		//	push reg_FL onto stack
	template<addrMode mode> void PLP();		//	pull reg_FL from stack

	//	register storing
	template<addrMode mode> void TAX();		//	reg_X = reg_A
	template<addrMode mode> void TAY();		//	reg_Y = reg_A
	template<addrMode mode> void TSX();		//	reg_X = reg_SP
	template<addrMode mode> void TXA();		//	reg_A = reg_X
	template<addrMode mode> void TYA();		//	reg_A = reg_Y
	template<addrMode mode> void TXS();		//	reg_SP = reg_X

	//	memory storing
	template<addrMode mode> void STA();		//	a = reg_A
	template<addrMode mode> void STX();		//	a = reg_X
	template<addrMode mode> void STY();		//	a = reg_Y

	//	branching
	template<addrMode mode> void BCC();		//	branch when carry == 0
	template<addrMode mode> void BCS();		//	branch when carry == 1
	template<addrMode mode> void BEQ();		//	branch when zero == 1
	template<addrMode mode> void BMI();		//	branch when negative == 1
	template<addrMode mode> void BNE();		//	branch when zero == 0
	template<addrMode mode> void BPL();		//	branch when negative == 0
	template<addrMode mode> void BVC();		//	branch when overflow == 0
	template<addrMode mode> void BVS();		//	branch when overflow == 1

	//	flag operations
	template<addrMode mode> void CLC();		//	clear carry flag
	template<addrMode mode> void CLD();		//	clear decimal flag
	template<addrMode mode> void CLI();		//	clear interrupt disable flag
	template<addrMode mode> void CLV();		//	clear overflow flag
	template<addrMode mode> void SEC();		//	set carry flag
	template<addrMode mode> void SED();		//	set decimal flag
	template<addrMode mode> void SEI();		//	set interrupt disable flag

	//	comparisons
	template<addrMode mode> void CMP();		//	reg_A - a
	template<addrMode mode> void CPX();		//	reg_X - a
	template<addrMode mode> void CPY();		//	reg_Y - a

	//	break
	template<addrMode mode> void BRK();

};

#endif