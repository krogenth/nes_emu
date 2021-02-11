#include "include\CPU.h"


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