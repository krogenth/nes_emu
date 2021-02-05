#ifndef IO_UTIL_H
#define IO_UTIL_H

#include <fstream>
#include <assert.h>

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

#endif