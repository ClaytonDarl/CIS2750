#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <stdbool.h>
#include <ctype.h>
#include "CalendarParser.h"

bool isComment(char* line);

bool isValidFile(char* fileName);

bool findCRLF(char* line);

bool findFolded(FILE* fp);

char* unFold(char* next, char* buffer, char* temp);

ICalErrorCode parseAlarm(FILE* fp, Calendar* theCal, Event* theEvt);

ICalErrorCode parseEvent(FILE* fp, Event* theEvent, Calendar* theCal);

int splitter(int x, int y);

FILE* writeEvent(FILE* fp, Event* theEvent);

FILE* writeProp(FILE* fp, Property* theProp);

FILE* writeAlarm(FILE* fp, Alarm* theAlarm);

ICalErrorCode verifyDate(DateTime theDate);

ICalErrorCode verifyEvtProp(Property* theProp);

ICalErrorCode verifyEvtPropList(Event* theEvent);

ICalErrorCode verifyCalProp(Property* theProp);

ICalErrorCode verifyAlarmProp(Property* theProp);

ICalErrorCode verifyAlarm(Alarm* theAlarm);

char* serverCreateCalendar(char* fileName);

char* serverEventList(char* fileName);

char* serverAddEvt(char* fileName, char* uid, char* startDate, char* startTime, char* summary);

char* propListtoJSon(Event* theEvent);

char* serverFindEvent(char* fileName, char* eventNum);

char* alarmListToJSon (Event* theEvent);

char* findAlarmList(char* fileName, char* eventNum);

char* validateFiles(char* fileName);

#endif