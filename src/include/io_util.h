#ifndef IO_UTIL_H
#define IO_UTIL_H

#include <fstream>
#include <assert.h>

#include <sstream>
#include <iomanip>

template<typename T>
T readRaw(std::ifstream& stream) {

	T val;
	stream.read(reinterpret_cast<char*>(&val), sizeof(T));
	assert(!stream.fail());
	return val;

}

template<typename T>
T readRaw(FILE* stream) {

	T val;
	fread(&val, sizeof(T), 1, stream);
	assert(!stream.fail());
	return val;

}

template<typename T>
void writeRaw(std::ofstream& stream, T& val) {

	stream.write(reinterpret_cast<char*>(&val), sizeof(T));
	assert(!stream.fail());

}

template<typename T>
void writeRaw(FILE* stream, T& val) {

	fwrite(&val, sizeof(T), 1, stream);
	assert(!stream.fail());

}

template <typename T>
inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {

	std::stringstream ss;
	ss << "0x" << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
	return ss.str();

}

#endif