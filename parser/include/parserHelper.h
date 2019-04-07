#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "CalendarParser.h"

/** Function to parse the file into individual lines, to be passed back as an array of strings
@pre File name cannot be empty or NULL. File must exist and be readable
@post Either:
      An array of strings containing each line of the file will be returned
      or
      An error occured while parsing the file, everything is freed and NULL is returned
@return an array of strings containing each line of the file
@param fileName contains the filename for the calendar
**/
char** parseFile(char* fileName, int* lineCount);

void freeStringList(char** stringList, int* lineCount);

ICalErrorCode calParser(int lineCounter, char** stringList);

Event* createEvent(int firstIndex, int secondIndex, char** parsedList);

Alarm* createAlarm(int firstIndex, int secondIndex, char** parsedList);

ICalErrorCode validateEvent(Event* event);

ICalErrorCode validateAlarm(Alarm* alarm);

#endif
