/*
 * OutputLogger.cpp
 *
 *  Created on: Jun 30, 2011
 *      Author: susan
 *
 *  The OutputLogger class is a subclass of the stringstream class.  It is used to create or suppress outputs
 *  to the console and to a log file.  By sending outputs to this class, the amount of output can be controlled
 *  from one plage
 */
// TODO:  This really should be a subclass of the ostream class, with a method overriding the << operator.


#include "OutputLogger.h"


/* Set up the files to be written, if necessary.  Create a string containing the time of day as the
 * name of the file, and write a start message to the file if needed.
 */
OutputLogger::OutputLogger(bool toFile, bool toConsole) {
	writeToFile = toFile;
	writeToConsole = toConsole;
	if ( writeToFile ) {
		time_t currTime = time(NULL);
		std::string timeStr = asctime(localtime(&currTime));
		*this << timeStr;
		std::string weekDay, month, time;
		int day, year;
		*this >> weekDay >> month >> day >> time >> year;
		this->str("");
		*this << "logs/" << weekDay << month << day << "_" << time << "_" << year << ".txt";
		outputFilename = this->str();
		fileOutput.open(outputFilename.c_str());
		fileOutput << "Log started: " << timeStr << std::endl;
		this->str("");
	}
}


/* If it is writing to a file, then close the file */
OutputLogger::~OutputLogger() {
	if (writeToFile) {
		fileOutput.close();
	}
}


/*
 * This writes the current contents of the stringstream to the console and file if either is in use.
 * In any case, it clears the contents of the stringstream
 */
void OutputLogger::writeLog() {
	if ( writeToFile ) {
		fileOutput << this->str() << std::endl;
	}
	if ( writeToConsole ) {
		std::cout << this->str() << std::endl;
	}
	this->str("");
}

