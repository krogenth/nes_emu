#ifndef CPU_H
#define CPU_H

#include <cinttypes>
#include <vector>

enum CPU_FLAGS : uint8_t {

	Carry     = 0b00000001,
	Zero      = 0b00000010,
	Interrupt = 0b00000100,
	Decimal   = 0b00001000,
	Unknown1  = 0b00010000,
	Unknown2  = 0b00100000,
	Overflow  = 0b01000000,
	Negative  = 0b10000000

};

/*
we need to forward delcare the CPUClass here in order to define the addrMode function pointer
with the function pointer, we can effectively pass in the addressing mdoes to the instructions
reducing significantly the amount of work required to code all opcodes
*/
class CPUClass;
typedef uint16_t(*addrMode)(CPUClass&);
class CartridgeClass;

class CPUClass {

public:
	CPUClass(CartridgeClass* _cartridge);
	~CPUClass();

protected:
	//	registers
	uint8_t reg_A = 0, reg_X = 0, reg_Y = 0, reg_FL = 0, reg_SP = 0;
	uint16_t reg_PC = 0;

	std::vector<uint8_t> RAM;

	//	store pointers to various compnents for calls
	CartridgeClass* cartridge = nullptr;
	//	we will need to store the APU and PPU here as well

	//	to handle the timing of instructions and PPU(PPU clock is 3x CPU clock(at least with NTSC, PAL is 2.4x))
	void cycle();

	//	used to determine what opcode to execute
	void execute();

	//	used to access the various components(APU, PPU, Cartridge, Peripheral)
	uint8_t access(uint16_t address, uint8_t data = 0, bool isWrite = false);

	//	helper functions
	void setFlag(CPU_FLAGS flag) { reg_FL |= flag; };
	void unsetFlag(CPU_FLAGS flag) { reg_FL &= ~(flag); };

	//	declare addressing modes as friends to this class
	friend inline uint16_t addrMode_IMM(CPUClass& CPU);		//	immediate mode:           1 byte, data to use is next byte
	friend inline uint16_t addrMode_IMP(CPUClass& CPU);		//	implied mode:             0 bytes, data is already with opcode, look at opcode PHA for why we might want this
	friend inline uint16_t addrMode_ACC(CPUClass& CPU);		//	accumulator mode:         0 bytes, instruction targets the accumulator register, so this mode is unneeded
	friend inline uint16_t addrMode_ABS(CPUClass& CPU);		//	absolute mode:            2 bytes, next two bytes specify the memory address
	friend inline uint16_t addrMode_ABX(CPUClass& CPU);		//	absolute X-indexed mode:  2 bytes
	friend inline uint16_t addrMode_ABY(CPUClass& CPU);		//	absolute Y-indexed mode:  2 bytes
	friend inline uint16_t addrMode_ZP0(CPUClass& CPU);		//	zero-page mode:           2 bytes(technically 1), address is in zero-page(0x00FF)
	friend inline uint16_t addrMode_ZPX(CPUClass& CPU);		//	zero-page x-indexed mode: 2 bytes(technically 1), address + X is in zero-page(0x00FF)
	friend inline uint16_t addrMode_ZPY(CPUClass& CPU);		//	zero-page y-indexed mode: 2 bytes(technically 1), address + Y is in zero-page(0x00FF)
	friend inline uint16_t addrMode_REL(CPUClass& CPU);		//	relative mode:            1 byte
	friend inline uint16_t addrMode_IND(CPUClass& CPU);		//	indirect mode:            2 bytes
	friend inline uint16_t addrMode_INX(CPUClass& CPU);		//	indirect x-indexed mode:  1 byte?
	friend inline uint16_t addrMode_INY(CPUClass& CPU);		//	indirect y-indexed mode:  1 byte?

	//	arithmatic
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
	template<addrMode mode> void ROL();		//	(a << 1) | ((a & 0b10000000) >> 7)
	template<addrMode mode> void ROR();		//	(a >> 1) | ((a & 0b00000001) << 7)

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