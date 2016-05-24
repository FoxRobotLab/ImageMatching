/*
 * OutputLogger.h
 *
 *  Created on: Jun 30, 2011
 *      Author: susan
 *
 *  The OutputLogger class is a subclass of the stringstream class.  It is used to create or suppress outputs
 *  to the console and to a log file.  By sending outputs to this class, the amount of output can be controlled
 *  from one plage
 */
// TODO:  This really should be a subclass of the ostream class, with a method overriding the << operator.

#ifndef OUTPUTLOGGER_H_
#define OUTPUTLOGGER_H_


#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<ctime>

using std::stringstream;
using std::string;
using std::ofstream;


class OutputLogger: public stringstream {
	// The name of the file to write to
	string outputFilename;
	// An output tream connected to the file
	ofstream fileOutput;
	// A boolean flag: should this object write to the log file?
	bool writeToFile;
	// A boolean flag: should this object write to the console?
	bool writeToConsole;

public:
	/* When created, the program must specify whether to write to a file or to the console, both, or neither */
	OutputLogger(bool toFile, bool toConsole);
	/* Destructor has to clean up the file-writing */
	virtual ~OutputLogger();

	/*
	 * Because we are not overriding the stringstream << operator, anything the program sends to this object
	 * will get stored away.  This method causes that information to be written to the console and/or file
	 */
	void writeLog();
};

#endif /* OUTPUTLOGGER_H_ */
