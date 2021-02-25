#include ".\CPU.h"


/*
ADDRESSING MODES

For all accesses, we must increment the program counter as well
Every addressing mode returns an address for the opcodes to use
*/
inline uint16_t addrMode_IMM(CPUClass& CPU) {



}

inline uint16_t addrMode_IMP(CPUClass& CPU) {



}

inline uint16_t addrMode_ACC(CPUClass& CPU) {



}

inline uint16_t addrMode_ABS(CPUClass& CPU) {



}

inline uint16_t addrMode_ABX(CPUClass& CPU) {



}

inline uint16_t addrMode_ABY(CPUClass& CPU) {



}

inline uint16_t addrMode_ZP0(CPUClass& CPU) {



}

inline uint16_t addrMode_ZPX(CPUClass& CPU) {



}

inline uint16_t addrMode_ZPY(CPUClass& CPU) {



}

inline uint16_t addrMode_REL(CPUClass& CPU) {



}

inline uint16_t addrMode_IND(CPUClass& CPU) {



}

inline uint16_t addrMode_INX(CPUClass& CPU) {



}


inline uint16_t addrMode_INY(CPUClass& CPU) {



}

/*
OPCODES
*/

//	arithmatic
template<addrMode mode> void CPUClass::ADC() {



}

template<addrMode mode> void CPUClass::SBC() {



}

//	bitwise
template<addrMode mode> void CPUClass::AND() {



}

template<addrMode mode> void CPUClass::BIT() {



}

template<addrMode mode> void CPUClass::ORA() {



}

template<addrMode mode> void CPUClass::EOR() {



}


//	bitwise shifts
template<addrMode mode> void CPUClass::ASL() {



}

template<addrMode mode> void CPUClass::LSR() {



}

template<addrMode mode> void CPUClass::ROL() {



}

template<addrMode mode> void CPUClass::ROR() {



}


//	Decrement/Increment
template<addrMode mode> void CPUClass::DEC() {



}

template<addrMode mode> void CPUClass::DEX() {

	this->reg_X--;

}

template<addrMode mode> void CPUClass::DEY() {

	this->reg_Y--;

}

template<addrMode mode> void CPUClass::INC() {



}

template<addrMode mode> void CPUClass::INX() {



}

template<addrMode mode> void CPUClass::INY() {



}


//	jumps
template<addrMode mode> void CPUClass::JMP() {



}

template<addrMode mode> void CPUClass::JSR() {



}

template<addrMode mode> void CPUClass::RTS() {



}

template<addrMode mode> void CPUClass::RTI() {



}


//	loads
template<addrMode mode> void CPUClass::LDA() {



}

template<addrMode mode> void CPUClass::LDX() {



}

template<addrMode mode> void CPUClass::LDY() {



}


//	no operation
template<addrMode mode> void CPUClass::NOP() {



}

//	stack operations
template<addrMode mode> void CPUClass::PHA() {



}

template<addrMode mode> void CPUClass::PLA() {



}

template<addrMode mode> void CPUClass::PHP() {



}

template<addrMode mode> void CPUClass::PLP() {



}


//	register storing
template<addrMode mode> void CPUClass::TAX() {



}

template<addrMode mode> void CPUClass::TAY() {



}

template<addrMode mode> void CPUClass::TSX() {



}

template<addrMode mode> void CPUClass::TXA() {



}

template<addrMode mode> void CPUClass::TYA() {



}

template<addrMode mode> void CPUClass::TXS() {



}


//	memory storing
template<addrMode mode> void CPUClass::STA() {



}

template<addrMode mode> void CPUClass::STX() {



}

template<addrMode mode> void CPUClass::STY() {



}


//	branching
template<addrMode mode> void CPUClass::BCC() {



}

template<addrMode mode> void CPUClass::BCS() {



}

template<addrMode mode> void CPUClass::BEQ() {



}

template<addrMode mode> void CPUClass::BMI() {



}

template<addrMode mode> void CPUClass::BNE() {



}

template<addrMode mode> void CPUClass::BPL() {



}

template<addrMode mode> void CPUClass::BVC() {



}

template<addrMode mode> void CPUClass::BVS() {



}


//	flag operations
template<addrMode mode> void CPUClass::CLC() {



}

template<addrMode mode> void CPUClass::CLD() {



}

template<addrMode mode> void CPUClass::CLI() {



}

template<addrMode mode> void CPUClass::CLV() {



}

template<addrMode mode> void CPUClass::SEC() {



}

template<addrMode mode> void CPUClass::SED() {



}

template<addrMode mode> void CPUClass::SEI() {



}


//	comparisons
template<addrMode mode> void CPUClass::CMP() {



}

template<addrMode mode> void CPUClass::CPX() {



}

template<addrMode mode> void CPUClass::CPY() {



}


//	break
template<addrMode mode> void CPUClass::BRK() {



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