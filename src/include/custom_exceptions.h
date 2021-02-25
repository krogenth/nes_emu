#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>	//	std::runtime_error

class CartridgeException : public std::runtime_error {

public:
	CartridgeException(const std::string& message) throw() : std::runtime_error(message) {



	}

};

class MapperException : public std::runtime_error {

public:
	MapperException(const std::string& message) throw() : std::runtime_error(message) {



	}

};

#endif