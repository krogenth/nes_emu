#ifndef ERRORLOG_H
#define ERRORLOG_H

#include <string>
#include <fstream>

// For timestamping error log messages
#include<time.h>

const std::string DEFAULT_LOG_FILE_NAME = "error.txt";

class ErrorLogClass {

public:
	ErrorLogClass();
	ErrorLogClass(std::string fileName); // will write to the given file rather than the default
	~ErrorLogClass();

	void Write(std::string message); // Writes the given error message to the error log file
	void SetLogFile(std::string fileName); // Set the log file to a new given value.
	bool LogExists(std::string fileName); // checks if a given log file exists

private:
	std::string logFileName; // name of the log file being used
	
	std::string TimeStamp(); // returns a timestamp of the current time.
	

};


#endif