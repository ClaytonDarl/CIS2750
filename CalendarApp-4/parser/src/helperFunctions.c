#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "helperFunctions.h"
#include "CalendarParser.h"


/*Checks if the line is a comment*/
bool isComment(char* line) {

	if (strcspn(line, ";") == 0) {
		return true;
	}

	return false;
}

/*Checks if the file contains .ics extension and if the file has any contents*/ //TESTED
bool isValidFile(char* fileName) {

    if (strstr(fileName, ".ics") != NULL) {
		return true;
    } else {
        return false;
    }

    FILE *fp;
    fp = fopen(fileName, "r");
    int c = fgetc(fp);

    if (c == EOF) {
        fclose(fp);
        return false;
    }

    if (fp == NULL) {
    	fclose(fp);
    	return false;
    }

    fclose(fp);
    return true;
}

/*Checks for the CR and LF flags inside of an .ics file*/ //TESTED
bool findCRLF(char* line) {

    //CR is there
    if(strstr(line, "\r") != NULL) {
        //CLRF is there
        if (strstr(line, "\r\n") != NULL) {
            return true;

            //no LF present
        } else {
			printf("no lf\n");
            return false;
        }
    } else {
		
        return false;
    }
}

/*Check for a folded line*/
bool findFolded(FILE* fp) {
    //check for CRLF' '
    if (fgetc(fp) == ' ' || fgetc(fp) == '\t') {
        //fseek(fp, -1, SEEK_CUR);//move the fp back to orig spot
        printf("???\n");
		return true;
    } else {
		//fseek(fp, -1, SEEK_CUR);
		return false;
	}
}

/*Takes 2 lines from the file and merges them*/
char* unFold(char* next, char* buffer, char* temp) {
	//printf("Buffer:%s", buffer);
	//printf("Next:%s", next);


		//remove the CRLF of the mainLine and save it in a temp string	
		memcpy(temp, buffer, strlen(buffer) - 2);
		strcat(temp, "\0");

		//strcat(temp, "\0");
		
		//printf("tempBefore:%s", temp);
		//append all but the space from next to tempLine
		strcat(temp, next + 1);
		strcat(temp, "\0");

		//printf("temp:%s", temp);

		return temp;
	
}

int splitter(int x, int y) {
	if (x < y) {
		return 1;
	} else if (y < x) {
		return -1;
	} else {
		return 0;
	}
}

ICalErrorCode parseAlarm(FILE* fp,Calendar* theCal, Event* theEvt){

	//bools for error checking
	bool foundAct = false;
	bool foundTrig = false;

	Alarm* alarm = malloc(sizeof(Alarm));
	alarm->properties = initializeList(&printProperty,&deleteProperty,&compareAlarms);
	char* line = malloc(sizeof(char) * 1000);

	//check if the next line is a comment!
	if(fgetc(fp) == ';') {
			fseek(fp, -1, SEEK_CUR);
			fgets(line, 1000, fp);
		} else {
			fseek(fp, -1, SEEK_CUR);
	}


	while(fgets(line, 1000, fp) != NULL) {
		
	//check for CRLF  flags
	if(findCRLF(line) == false) {
		printf("%s\n line has no CRLF3", line);
		free(line);
		return INV_FILE;
	}
		
		if (strstr(line, "END:VALARM") != NULL) {

			//check for trigger and action
			if(foundTrig == false || foundAct == false) {
				free(line);
				return INV_ALARM;
			}

			//check for a comment after the END:VALARM
			if(fgetc(fp) == ';') {
					fseek(fp, -1, SEEK_CUR);
					fgets(line, 1000, fp);
				} else {
					fseek(fp, -1, SEEK_CUR);
			}

			free(line);
			insertBack(theEvt->alarms, alarm);
			return OK; 
			
		} else if (strstr(line, "TRIGGER") != NULL) {

			foundTrig = true;
			char* buffer = NULL;
			buffer = calloc(1, sizeof(char*) * strlen(line));
			strcpy(buffer, line);

			//get the info of trigger
			int semiSplit = strcspn(buffer, ";");
			int colonSplit = strcspn(buffer, ":");
			char* trig = NULL;
			trig = calloc(1, sizeof(char) * 100);

			if(splitter(semiSplit, colonSplit) == 1) {
			//copy from : to the end of the line
			strncpy(trig, buffer + semiSplit + 1, strlen(buffer));

			//remove the CRLF from trig
				if (findCRLF(trig) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(trig, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, trig, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(trig, temp);
				free(temp);
				}

			} else if (splitter(semiSplit, colonSplit) == -1 || splitter(semiSplit, colonSplit) == 0) {
				//copy from : to the end of the line
			strncpy(trig, buffer + colonSplit + 1, strlen(buffer));

			//remove the CRLF from trig
				if (findCRLF(trig) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(trig, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, trig, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(trig, temp);
				free(temp);
				}
			}


			//allocate just enough mem for the string and \0
			alarm->trigger = malloc((sizeof(char) * (strlen(trig) + 1)));
			strcpy(alarm->trigger, trig);

		
			free(trig);
			free(buffer);
			
			

			//check if the next line is a comment!
			if(fgetc(fp) == ';') {
				fseek(fp, -1, SEEK_CUR);
				fgets(line, 1000, fp);
			} else {
				fseek(fp, -1, SEEK_CUR);
			}
			
		} else if (strstr(line, "ACTION") != NULL) {

			foundAct = true;
			char* buffer = NULL;
			buffer = calloc(1, sizeof(char*) * strlen(line));
			strcpy(buffer, line);

			int nameSplit = 0;
			nameSplit = strcspn(buffer, ":");
			int descLen =  0;
			descLen = strcspn(buffer, "\r\n");
			char* act = calloc(1, sizeof(char) * (descLen + 6) + 4); //mem issues here
			strncpy(act, buffer + (nameSplit + 1), descLen + 1);

			//remove the CRLF from trig
				if (findCRLF(act) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(act, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp,act, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(act, temp);
				free(temp);
				}

			strcpy(alarm->action, act); 

		
			free(act);
			free(buffer);
			

			//check if the next line is a comment!
			if(fgetc(fp) == ';') {
				fseek(fp, -1, SEEK_CUR);
				fgets(line, 1000, fp);
			} else {
				fseek(fp, -1, SEEK_CUR);
			}

		} else if (strstr(line, "DURATION") != NULL) {
			char* buffer = calloc(1, sizeof(char) * 1000) ;
			strcat(buffer, line);

			char c = fgetc(fp);
			//Check for and deal with a folded line
			while (c ==  ' ' || c == '\t') {
				fseek(fp, -1, SEEK_CUR);
				char* temp = calloc(1, sizeof(char) * 1000);
		
				/*get the next line*/
				char* nextLine = calloc(1, sizeof(char*) * 1000);
				fgets(nextLine, 1000, fp);

				strcpy(buffer, unFold(nextLine, buffer, temp));
		
				free(temp);
				free(nextLine);

				c = fgetc(fp);
			}
		
			fseek(fp, -1, SEEK_CUR); //seek back after line folding is finished

			int bufferLen = strlen(buffer);
			int colonSplit = strcspn(buffer, ":");

			//Get the property name
				char* name = NULL;
				name = calloc(1,(sizeof(char) * colonSplit + 1));
				strncpy(name, buffer, colonSplit);

				if (findCRLF(name) == true) {
			
				//Remove the cr
				int crFlag = strcspn(buffer, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, buffer, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(buffer, temp);
				free(temp);
				}

				if(strstr(name, "\0") != NULL) {
					//printf("adding the null term\n");
					strcat(name, "\0");
				}

				//Check for a null name
				if(strlen(name) < 1) {
					free(buffer);
					free(name);
					return INV_ALARM;
				}

				Property* newProp = calloc(1, sizeof(Property) + (bufferLen + 1) * sizeof(char));	
				strcpy(newProp->propName, "\0");
				strcpy(newProp->propDescr, "\0");
				memcpy(newProp->propName, name, strlen(name)); //Properly init a property to fix the mem
				//printf("duration specific propname colon: %s\n", newProp->propName);


				/*----------------------------------Handle the Property Description----------------------------------*/
				//Get the property description
				int descLen = strcspn(buffer, "\r\n");
				char* desc = calloc(1, (sizeof(char) * descLen) + 3); //no malloced right
				memcpy(desc, buffer + colonSplit + 1, bufferLen);

				//remove the CRLF from the desc
				if (findCRLF(desc) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(desc, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, desc, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(desc, temp);
				free(temp);
				}

				//check for a null desc
				if(strlen(desc) < 1) {
					free(buffer);
					free(name);
					free(desc);
					free(newProp);
					return INV_ALARM;
				}

				

				strcpy(newProp->propDescr, desc);
				//printf("AlmpropDesc colon: %s\n", newProp->propDescr);

				insertBack(alarm->properties, newProp); //something not fully init here

				free(desc);
				free(name);
				free(buffer);

				//check if the next line is a comment!
			if(fgetc(fp) == ';') {	
				fseek(fp, -1, SEEK_CUR);
				fgets(line, 1000, fp);
			} else {
				fseek(fp, -1, SEEK_CUR);
			}
			

		} else {
			char* buffer = NULL;
			buffer = calloc(1, sizeof(char)* 1000);
			char * const toFree = buffer;
			strcat(buffer, line);

			//printf("alarm buffer: %s", buffer);
			
			
			char c = fgetc(fp);
			//Check for and deal with a folded line
			while (c ==  ' ' || c == '\t') {
				fseek(fp, -1, SEEK_CUR);
				char* temp = calloc(1, sizeof(char) * 1000);
		
				/*get the next line*/
				char* nextLine = calloc(1, sizeof(char*) * 1000);
				fgets(nextLine, 1000, fp);

				strcpy(buffer, unFold(nextLine, buffer, temp));
		
				free(temp);
				free(nextLine);

				c = fgetc(fp);
			}
		
			fseek(fp, -1, SEEK_CUR); //seek back after line folding is finished
			
			//determine the type of delimiter to use
			int bufferLen = strlen(buffer);
			int semiSplit = strcspn(buffer, ";");
			int colonSplit = strcspn(buffer, ":");
		

			//Determines what delimiter to use
			if (splitter(semiSplit, colonSplit) == 1) {
				
				//Get the property name
				char* name = calloc(1,(sizeof(char) * semiSplit + 1));
				strncpy(name, buffer, semiSplit);
				strcat(name, "\0");

				//Check for a null name
				if(strlen(name) < 1) {
					free(buffer);
					free(name);
					return INV_ALARM;
				}


				//make a new property
				Property* newProp = calloc(1, sizeof(Property) + (bufferLen + 1) * sizeof(char));	
				strcpy(newProp->propName, "\0");
				strcpy(newProp->propDescr, "\0");
				memcpy(newProp->propName, name, strlen(name)); 

				//printf("Almpropname semi: %s\n", newProp->propName);

				

				//Get the property description
				int descLen = strcspn(buffer, "\r\n");
				char* desc = NULL;
				desc = calloc(1, (sizeof(char) * descLen) + 3); //no malloced right
				memcpy(desc, buffer + semiSplit + 1, bufferLen);
				

				if (findCRLF(desc) == true) {
				
					//Remove the crlf
					int crFlag = strcspn(desc, "\r\n");
					char* temp = calloc(1, sizeof(char) * strlen(desc));
					strncpy(temp, desc, crFlag);
					//add the \0
					strcat(temp, "\0");
					strcpy(desc, temp);
					free(temp);
				}

				//check for a null desc
				if(strlen(desc) < 1) {
					free(buffer);
					free(name);
					free(desc);
					free(newProp);
					return INV_ALARM;
				}

				

				strcpy(newProp->propDescr, desc);
				
				//printf("AlmpropDesc semi: %s\n", newProp->propDescr);

				insertBack(alarm->properties, newProp); //something not fully init here
				

				free(desc);
				free(name);
				free(toFree);
			} else if (splitter(semiSplit, colonSplit) == -1 || splitter(semiSplit, colonSplit) == 0) {
				
				//Get the property name
				char* name = NULL;
				name = calloc(1,(sizeof(char) * colonSplit + 1));
				strncpy(name, buffer, colonSplit);
				if (findCRLF(name) == true) {
			
				//Remove the cr
				int crFlag = strcspn(buffer, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, buffer, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(buffer, temp);
				free(temp);
				}

				//Check for a null name
				if(strlen(name) < 1) {
					free(buffer);
					free(name);
					return INV_ALARM;
				}

				
				Property* newProp = calloc(1, sizeof(Property) + (bufferLen + 1) * sizeof(char));	
				strcpy(newProp->propName, "\0");
				strcpy(newProp->propDescr, "\0");
				memcpy(newProp->propName, name, strlen(name)); //Properly init a property to fix the mem
				//printf("Almpropname colon: %s\n", newProp->propName);


				/*----------------------------------Handle the Property Description----------------------------------*/
				//Get the property description
				int descLen = strcspn(buffer, "\r\n");
				char* desc = calloc(1, (sizeof(char) * descLen) + 3); //no malloced right
				memcpy(desc, buffer + colonSplit + 1, bufferLen);

				//remove the CRLF from the desc
				if (findCRLF(desc) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(desc, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, desc, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(desc, temp);
				free(temp);
				}

				//check for a null desc
				if(strlen(desc) < 1) {
					free(buffer);
					free(name);
					free(desc);
					free(newProp);
					return INV_ALARM;
				}

				

				strcpy(newProp->propDescr, desc);
				//printf("AlmpropDesc colon: %s\n", newProp->propDescr);

				insertBack(alarm->properties, newProp); //something not fully init here

				free(desc);
				free(name);
				free(toFree);


			}
			//check if the next line is a comment!
			if(fgetc(fp) == ';') {	
				fseek(fp, -1, SEEK_CUR);
				fgets(line, 1000, fp);
			} else {
				fseek(fp, -1, SEEK_CUR);
			}
		}
	
	}
	free(line);
		
	return OK;
}

ICalErrorCode parseEvent(FILE* fp, Event* theEvent, Calendar* theCal){
	

	char* line = malloc(sizeof(char) * 1000);
	
	bool hasUID = false;
	bool hasDTSTAMP = false;

	//check if the next line is a comment , move to it so we can skip it if yes
	//while (isComment(line) == true) {
	//	fgets(line, 1000, fp);
	//}


	while (fgets(line, 1000, fp) != NULL) {
		
		//check for CRLF  flags
		if(findCRLF(line) == false) {
			printf("%s\n line has no CRLF4", line);
			free(line);
			return INV_FILE;
		}
		

		if (strstr(line, "UID") != NULL) { //working -- needs error checking

			hasUID = true;
			char* buffer = NULL;
			buffer = calloc(1, sizeof(char)* 1000);
			char * const toFree = buffer;

    		int len = 0;
    		len = strcspn(line, ":"); //finds index of first :
    		int endIndex = 0;
    		endIndex = strcspn(line, "\r\n") + 1;

    		strncpy(buffer, line + len + 1, endIndex - len);

			if (findCRLF(buffer) == true) {
		
			//Remove the cr
			int crFlag = strcspn(buffer, "\r\n");
			char* temp = calloc(1, sizeof(char) * strlen(buffer));
			strncpy(temp, buffer, crFlag);
			//add the \0
			strcat(temp, "\0");
			strcpy(buffer, temp);
				free(temp);
			}

			


			strcpy(theEvent->UID, buffer);

			free(toFree);
			
			if(fgetc(fp) == ';') {
				fseek(fp, -1, SEEK_CUR);
				fgets(line, 1000, fp);
			} else {
				fseek(fp, -1, SEEK_CUR);
			}

    	} else  if (strstr(line, "DTSTAMP") != NULL) {
			
    		hasDTSTAMP = true;

			char* buffer = NULL;
			buffer = calloc(1, sizeof(char)* 1000);
			

			int len = 0;
			len = strcspn(line, ":"); //find index of first :
			int endIndex = 0;
			endIndex = strcspn(line, "\r\n");

			strncpy(buffer, line + len + 1, endIndex - len);

			if(strstr(buffer, "T") == NULL || strcspn(buffer, "T") != 8) {
				free(line);
				free(buffer);
				return INV_DT;
			}

			char c = fgetc(fp);
			//Check for and deal with a folded line
			while (c ==  ' ' || c == '\t') {
				fseek(fp, -1, SEEK_CUR);
				char* temp = calloc(1, sizeof(char) * 1000);
		
				/*get the next line*/
				char* nextLine = calloc(1, sizeof(char*) * 1000);

				fgets(nextLine, 1000, fp);

				strcpy(buffer, unFold(nextLine, buffer, temp));
		
				free(temp);
				free(nextLine);

				c = fgetc(fp);
			}
		
			fseek(fp, -1, SEEK_CUR); //seek back after line folding is finished



			//Split string at 8 for date
			char* date = NULL;
			date = calloc(1, sizeof(char*) * 9);
			strncpy(date, buffer, 8);


			//Split after 8 to where T is
			int Tindex = 0;
			Tindex = strcspn(buffer, "T");
			char* startTime = NULL;
			startTime = calloc(1, sizeof(char*) * 7);
			
			
			memcpy(startTime, buffer + Tindex + 1, 6);

			//Add the \0 if its not there
			if(strstr(date, "\0") == NULL) {
				strcat(date, "\0");
			}
			
			if(strstr(startTime, "\0") == NULL) {
				strcat(startTime, "\0");
			}

			DateTime* newDate = malloc(sizeof(DateTime));
			newDate->date[0] = '\0';
			newDate->time[0] = '\0';
			//Have a conditional jump or move depends on unitialised values here for both DT members
			strcpy(newDate->date, date);//conditonal jump error here
			strcpy(newDate->time, startTime);//conditional jump error here
			
			//Get the UTC value
			if (strstr(buffer, "Z") != NULL) {
				newDate->UTC = true;
			} else {
				newDate->UTC = false;
			}

			//Add the error handling for dupe dates

			
			theEvent->creationDateTime = *newDate;
		
			free(newDate);
			free(date);
			free(startTime);
			free(buffer);
			//check if the next line is a comment!
			if(fgetc(fp) == ';') {
				fseek(fp, -1, SEEK_CUR);
				fgets(line, 1000, fp);
			} else {
				fseek(fp, -1, SEEK_CUR);
			}


   		} else  if (strstr(line, "DTSTART") != NULL) {
		   
			char* buffer = calloc(1, sizeof(char)* 1000);


			char c = fgetc(fp);
			//Check for and deal with a folded line
			while (c ==  ' ' || c == '\t') {
				fseek(fp, -1, SEEK_CUR);
				char* temp = calloc(1, sizeof(char) * 1000);
		
				/*get the next line*/
				char* nextLine = calloc(1, sizeof(char*) * 1000);

				fgets(nextLine, 1000, fp);

				strcpy(line, unFold(nextLine, line, temp));
		
				free(temp);
				free(nextLine);

				c = fgetc(fp);
			}
		
			fseek(fp, -1, SEEK_CUR); //seek back after line folding is finished

			int len = 0;
    		len = strcspn(line, ":"); //finds index of first :
    		int endIndex = 0;
    		endIndex = strcspn(line, "\r\n") + 1;

    		strncpy(buffer, line + len + 1, endIndex);

			DateTime* newDate = NULL;
			newDate = malloc(sizeof(DateTime));
			

			//Split string at 8 for date, add date to newDate
			char* date = NULL;
			date = calloc(1, sizeof(char*) * 9);
			strncpy(date, buffer, 8);
			strncpy(newDate->date, date, 9); //conditional jump error here

			//Split after 8 to where T is, add time to newDate
			int Tindex = 0;
			Tindex = strcspn(buffer, "T");
			char* startTime = NULL;
			startTime = calloc(1, sizeof(char*) * 7);
			memcpy(startTime, buffer + Tindex + 1, 6);
			strcpy(newDate->time, startTime); //conditional jump error here

			//Add the \0 if its not there
			if(strstr(newDate->date, "\0") == NULL) {
				strcat(newDate->date, "\0");
			}
			
			if(strstr(newDate->time, "\0") == NULL) {
				strcat(newDate->time, "\0");
			}	
			
			//Get the UTC value, add it to newDate
			if (strstr(buffer, "Z") != NULL) {
				newDate->UTC = true;
			} else {
				newDate->UTC = false;
			}

			//printf("Date: %s\n", newDate->date);
			//printf("Time: %s\n", newDate->time);
			theEvent->startDateTime = *newDate;

			free(newDate);
			free(date);
			free(startTime);
			free(buffer);

			//check if the next line is a comment!
			if(fgetc(fp) == ';') {
				fseek(fp, -1, SEEK_CUR);
				fgets(line, 1000, fp);
			} else {
				fseek(fp, -1, SEEK_CUR);
			}
			
    	} else  if (strstr(line, "END:VEVENT") != NULL) {
			

			//check for UID and DTSTAMP
    		if( hasUID == false || hasDTSTAMP == false) {
    			free(line);
    			return INV_EVENT;
    		}

			insertBack(theCal->events, (void*)theEvent);
			free(line);
			return OK;
		} else if (strstr(line, "BEGIN:VALARM") != NULL) {
			ICalErrorCode status = parseAlarm(fp, theCal, theEvent);

			if (status != OK) {
				free(line);
				return status;
			}
		} else {
			char* buffer = NULL;
			buffer = calloc(1, sizeof(char)* 1000);
			strcat(buffer, line);

			
			
			//if I find a stray begin:vevent kill it
			if(strstr(buffer, "BEGIN:VEVENT") != NULL) {
				free(line);
				free(buffer);
				return INV_EVENT;
			}
			//printf("buffer before unFold: %s", buffer);
			
			char c = fgetc(fp);
			//Check for and deal with a folded line
			while (c ==  ' ' || c == '\t') {

				

				fseek(fp, -1, SEEK_CUR);
				char* temp = calloc(1, sizeof(char) * 1000);
		
				/*get the next line*/
				char* nextLine = calloc(1, sizeof(char*) * 1000);

				fgets(nextLine, 1000, fp);

				if ((strlen(buffer) + strlen(nextLine) > 100)) {
					buffer = realloc(buffer, sizeof(char*) * (strlen(buffer) + 200) + 1);
					temp = realloc(temp, sizeof(char*) * (strlen(buffer) + 200) + 1);
				}


				strcpy(buffer, unFold(nextLine, buffer, temp));
		
				free(temp);
				free(nextLine);

				c = fgetc(fp);
			}
		
			fseek(fp, -1, SEEK_CUR); //seek back after line folding is finished

			//printf("Buffer: %s", buffer);
			
			//determine the type of delimiter to use
			int bufferLen = strlen(buffer);
			int semiSplit = strcspn(buffer, ";");
			int colonSplit = strcspn(buffer, ":");
		

			//Determines what delimiter to use
			if (splitter(semiSplit, colonSplit) == 1) {
				
				//Get the property name
				char* name = calloc(1,(sizeof(char) * semiSplit + 1));
				strncpy(name, buffer, semiSplit);
				strcat(name, "\0");
				

				//Check for a null name
				if(strlen(name) < 1) {
					
					free(buffer);
					free(name);
					free(line);
					return INV_EVENT;
				}


				//make a new property
				Property* newProp = calloc(1, sizeof(Property) + (bufferLen + 1) * sizeof(char));	
				strcpy(newProp->propName, "\0");
				strcpy(newProp->propDescr, "\0");
				memcpy(newProp->propName, name, strlen(name)); 

				//printf("propname semi: %s\n", newProp->propName);

				

				//Get the property description
				int descLen = strcspn(buffer, "\r\n");
				char* desc = NULL;
				desc = calloc(1, (sizeof(char) * descLen) + 3); //no malloced right
				memcpy(desc, buffer + semiSplit + 1, bufferLen);
				if (findCRLF(desc) == true) {
				
				//Remove the crlf
				int crFlag = strcspn(desc, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(desc));
				strncpy(temp, desc, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(desc, temp);
				free(temp);
				
				}

				//check for a null desc
				if(strlen(desc) < 1) {
					
					free(buffer);
					free(name);
					free(desc);
					free(newProp);
					free(line);
					return INV_EVENT;
				}

				

				strcpy(newProp->propDescr, desc);
				
				//printf("propDesc semi: %s\n", newProp->propDescr);

				insertBack(theEvent->properties, newProp); //something not fully init here
				
				free(name);
				free(desc);
				free(buffer);
			} else if (splitter(semiSplit, colonSplit) == -1 || splitter(semiSplit, colonSplit) == 0) {
				
				//Get the property name
				char* name = NULL;
				name = calloc(1,(sizeof(char) * colonSplit + 1));
				strncpy(name, buffer, colonSplit);
				strcat(name, "\0");

				//printf("name: %s\n", name);
				if (findCRLF(name) == false) {
			
				//Remove the cr
				int crFlag = strcspn(buffer, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, buffer, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(buffer, temp);
				free(temp);
				}

				//Check for a null name
				if(strlen(name) < 1) {
					
					free(buffer);
					free(name);
					free(line);
					return INV_EVENT;
				}

				Property* newProp = calloc(1, sizeof(Property) + (bufferLen + 1) * sizeof(char));	
				memcpy(newProp->propName, name, strlen(name)); //Properly init a property to fix the mem
				//printf("propname: %s\n", newProp->propName);


				/*----------------------------------Handle the Property Description----------------------------------*/
				//Get the property description
				int descLen = strcspn(buffer, "\r\n");
				char* desc = calloc(1, (sizeof(char) * descLen) + 3); //no malloced right
				memcpy(desc, buffer + colonSplit + 1, bufferLen);

				//remove the CRLF from the desc
				if (findCRLF(desc) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(desc, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, desc, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(desc, temp);
				free(temp);
				}

				//check for a null desc
				if(strlen(desc) < 1) {
					
					free(buffer);
					free(name);
					free(desc);
					free(newProp);
					free(line);
					return INV_EVENT;
				}

				

				strcpy(newProp->propDescr, desc);
				//printf("propDesc: %s\n", newProp->propDescr);

				insertBack(theEvent->properties, newProp); //something not fully init here

				free(desc);
				free(name);
				free(buffer);


			}
	
			//check if the next line is a comment!
			if(fgetc(fp) == ';') {
					fseek(fp, -1, SEEK_CUR);
					fgets(line, 1000, fp);
				} else {
					fseek(fp, -1, SEEK_CUR);
			}
		
			
		
	
	}

	}
	free(line);
	

	//check if the DTSTAMP is there
	

	



	return OK;
}

/*Write a property to a file*/
FILE* writeProp (FILE* fp, Property* theProp) {

	//printf("NAME: %s DESCRIPTION: %s\n", theProp->propName, theProp->propDescr);

	//write the property to the file
	char* prop = calloc(1, sizeof(char*) * (strlen(theProp->propName) + strlen(theProp->propDescr)) + 4); //conditional jump here
	strcpy(prop, theProp->propName); //unconditional jump here
	strcat(prop, ":");
	strcat(prop, theProp->propDescr);
	if(findCRLF(theProp->propDescr) == false) {
		//printf("%s had no CRLF\n", theProp->propDescr);
		strcat(prop, "\r\n");
	}
	
	strcat(prop, "\0");

	fwrite(prop, sizeof(char) * strlen(prop), 1, fp);
	free(prop);

	return fp;
}

FILE* writeAlarm(FILE* fp, Alarm* theAlarm) {

	//write the action to the file
	char* action = calloc(1, sizeof(char*) * (strlen("ACTION:") + strlen(theAlarm->action)) + 3);
	strcpy(action, "ACTION:");
	strcat(action, theAlarm->action);
	strcat(action, "\r\n\0");

	fwrite(action, sizeof(char) * strlen(action), 1, fp);
	free(action);

	//write the trigger to the file
	char* trig = calloc(1, sizeof(char*) * (strlen("TRIGGER:") + strlen(theAlarm->trigger)) + 3);
	strcpy(trig, "TRIGGER:");
	strcat(trig, theAlarm->trigger);
	strcat(trig, "\r\n\0");

	fwrite(trig, sizeof(char) * strlen(trig), 1, fp);
	free(trig);

	//write the property list to the file
	if (theAlarm->properties != NULL) {
		ListIterator propIt = createIterator(theAlarm->properties);
		propIt.current = theAlarm->properties->head;

		while (propIt.current != NULL) {
			Property* wProp = propIt.current->data;

			fp = writeProp(fp, wProp);

			propIt.current = propIt.current->next;
		}
	}

	return fp;
}

/*Write an event to a file, returns the file pointer*/
FILE* writeEvent (FILE* fp, Event* theEvent) {

	//write the UID to the file
	char* UID = calloc(1, sizeof(char*) * (strlen(theEvent->UID) + strlen("UID:")) + 3);
	strcpy(UID, "UID:");
	strcat(UID, theEvent->UID);
	strcat(UID, "\r\n\0");

	fwrite(UID, sizeof(char) * strlen(UID), 1, fp);
	free(UID);

	//write the creationDateTime
	char* creationDT = calloc(1, sizeof(char*) * (9 + 7 + 2 + strlen("DTSTAMP:")) + 3);
	strcpy(creationDT, "DTSTAMP:");
	strcat(creationDT, theEvent->creationDateTime.date);
	strcat(creationDT, "T");
	strcat(creationDT, theEvent->creationDateTime.time);
	if(theEvent->creationDateTime.UTC == true) {
		strcat(creationDT, "Z");
	}
	strcat(creationDT, "\r\n\0");
	//printf("CreationDT: %s", creationDT);
	fwrite(creationDT, sizeof(char) * strlen(creationDT), 1, fp);
	free(creationDT);


	//write the startdateTime
	char* startDT = calloc(1, sizeof(char*) * (9 + 7 + 2 + strlen("DTSTART:")) + 3);
	strcpy(startDT, "DTSTART:");
	//printf("oi %s\n", theEvent->startDateTime.date);
	strcat(startDT, theEvent->startDateTime.date);
	strcat(startDT, "T");
	//printf("yeet %s\n", theEvent->startDateTime.time);
	strcat(startDT, theEvent->startDateTime.time);
	if(theEvent->startDateTime.UTC == true) {
		strcat(startDT, "Z");
	}
	strcat(startDT, "\r\n\0");
	//printf("StartDT: %s", startDT);
	fwrite(startDT, sizeof(char) * strlen(startDT), 1, fp);
	free(startDT);

	//iterate over the alarm list
	if(theEvent->alarms != NULL) {

		//make an iterator for the alarm list
		ListIterator alarmIt = createIterator(theEvent->alarms);
		//assign the current to the head of the list
		alarmIt.current = theEvent->alarms->head;

		while(alarmIt.current != NULL) {
			Alarm* wAlarm = alarmIt.current->data;

			fwrite("BEGIN:VALARM\r\n", sizeof(char) * strlen("BEGIN:VALARM\r\n"), 1, fp);

			fp = writeAlarm(fp, wAlarm);

			fwrite("END:VALARM\r\n", sizeof(char) * strlen("END:VALARM\r\n"), 1, fp);

			alarmIt.current = alarmIt.current->next;
		}
	}

	//write the property list to the file
	if (theEvent->properties != NULL) {
		ListIterator propIt = createIterator(theEvent->properties);
		propIt.current = theEvent->properties->head;

		while (propIt.current != NULL) {
			Property* wProp = propIt.current->data;

			fp = writeProp(fp, wProp);

			propIt.current = propIt.current->next;
		}
	}
	return fp;
}

/*Verify the contents of a DateTime struct*/
ICalErrorCode verifyDate(DateTime theDate) {
	
	//check for empty date
	if (strlen(theDate.date) <= 1) {  //uncondit jump here
		return INV_DT;
	}

	//check for too large date
	if (strlen(theDate.date) > 9) {
		return INV_DT;
	}

	//check for empty time
	if (strlen(theDate.time) <= 1) {
		return INV_DT;
	}

	//check for too long time
	if(strlen(theDate.time) > 7) {
		return INV_DT;
	}

	return OK;
}

/*Veridy the general property params of an event property*/
ICalErrorCode verifyEvtProp(Property* theProp) {
	
	//check for empty propName
	if (strlen(theProp->propName) <= 1) {
		//printf("empty propname\n");
		return INV_EVENT;
	}

	//check for too large propName
	if (strlen(theProp->propName) > 200) {
		//printf("large propname\n");
		return INV_EVENT;
	}

	//check for empty propDescr
	if (strlen(theProp->propDescr) <= 0) {
		//printf("%s has empty propDesc\n", theProp->propName);
		return INV_EVENT;
	}

	if(strstr(theProp->propName, "CALSCALE") != NULL) {
		//printf("has calscale\n");
		return INV_EVENT;
	}

	if(strstr(theProp->propName, "DTSTART") != NULL) {
		//printf("dupe dts\n");
		return INV_EVENT;
	}

	return OK;
}	

/*Verify the list of properties within an Event*/
ICalErrorCode verifyEvtPropList(Event* theEvent) {

	//Ints to track property count
	int hasClass = 0;
	int hasCreated = 0;
	int hasDescription = 0;
	int hasGeo = 0;
	int hasLMod = 0;
	int hasLoca = 0;
	int hasOrg = 0;
	int hasPrior = 0;
	int hasSeq = 0;
	int hasStatus = 0;
	int hasSum = 0;
	int hasTran = 0;
	int hasURL = 0;
	int hasRecuid = 0;
	int hasMethod = 0;
	int hasDur = 0;

	/*Handle the DTEND and duration case*/

	//create an iterator for the property list
	ListIterator propIt = createIterator(theEvent->properties);
	propIt.current = theEvent->properties->head;

	ICalErrorCode propCheck = OK;

	//loop over the prop list
	while(propIt.current != NULL) {
		Property* tempProp = propIt.current->data;

		//verify the general property stuff
		propCheck = verifyEvtProp(tempProp);

		if(propCheck != OK) {
			return propCheck;
		}

		/*Check the name of the property*/
		if(strstr(tempProp->propName, "CLASS") != NULL) {
			hasCreated++;
		} else if (strstr(tempProp->propName, "CREATED") != NULL) {
			hasCreated++;
		} else if (strstr(tempProp->propName, "DESCRIPTION") != NULL) {
			hasDescription++;
		} else if (strstr(tempProp->propName, "GEO") != NULL) {
			hasGeo++;
		} else if (strstr(tempProp->propName, "LAST-MODIFIED") != NULL) {
			hasLMod++;
		} else if (strstr(tempProp->propName, "LOCATION") != NULL) {
			hasLoca++;
		} else if (strstr(tempProp->propName, "ORGANIZER") != NULL) {
			hasOrg++;
		} else if (strstr(tempProp->propName, "PRIORITY") != NULL) {
			hasPrior++;
		} else if (strstr(tempProp->propName, "SEQUENCE") != NULL) {
			hasSeq++;
		} else if (strstr(tempProp->propName, "STATUS") != NULL) {
			hasStatus++;
		} else if (strstr(tempProp->propName, "SUMMARY") != NULL) {
			hasSum++;
		} else if (strstr(tempProp->propName, "TRANSP") != NULL) {
			hasTran++;
		} else if (strstr(tempProp->propName, "URL") != NULL) {
			hasURL++;
		} else if (strstr(tempProp->propName, "RECURRENCE-ID") != NULL) {
			hasRecuid++;
		} else if (strstr(tempProp->propName, "DTEND") != NULL) {
			hasDur++;
		}

		propIt.current = propIt.current->next;
	}

	//check if method and description appear
	if(hasMethod == 1 && hasDescription == 1) {
		//printf("method desc\n");
		return INV_EVENT;
	}

	/*----------------------------CHECK FOR DUPLICATE PROPERTIES----------------------*/
	if(hasClass > 1) {
		//printf("dupe class\n");
		return INV_EVENT;
	}

	if(hasDescription > 1){
		//printf("dupe desc\n");
		return INV_EVENT;
	}

	if(hasGeo > 1){
		//printf("dupe geo\n");
		return INV_EVENT;
	}

	if(hasLMod > 1){
		//printf("dupe lmod\n");
		return INV_EVENT;
	}

	if(hasOrg > 1){
		//printf("dupe org\n");
		return INV_EVENT;
	}

	if(hasPrior > 1){
		//printf("dupe prior\n");
		return INV_EVENT;
	}

	if(hasSeq > 1){
		//printf("dupe seq\n");
		return INV_EVENT;
	}

	if(hasStatus > 1){
		//printf("dupe stat\n");
		return INV_EVENT;
	}

	if(hasSum > 1){
		//printf("dupe sum\n");
		return INV_EVENT;
	}

	if(hasTran > 1){
		//printf("dupe tran\n");
		return INV_EVENT;
	}

	if(hasURL > 1){
		//printf("dupe url\n");
		return INV_EVENT;
	}

	if(hasRecuid > 1){
		//printf("dupe revuid\n");
		return INV_EVENT;
	}

	if(hasMethod > 1){
		//printf("dupe method\n");
		return INV_EVENT;
	}
	/*-------------------CHECK FOR DUPLICATE PROPERTIES END---------------------*/
	return OK;
}

/*Verify a Calendar property*/
ICalErrorCode verifyCalProp(Property* theProp) {

	//check for empty propName
	if (strlen(theProp->propName) <= 1) {
		return INV_CAL;
	}

	//check for too large propName
	if (strlen(theProp->propName) > 200) {
		return INV_CAL;
	}

	//check for empty propDescr
	if (strlen(theProp->propDescr) <= 1) {
		return INV_CAL;
	}

	//check if the properties are valid calendar ones
	if(strstr(theProp->propName, "CALSCALE") == NULL && strstr(theProp->propName, "METHOD") == NULL) {
		return INV_CAL;
	}


	return OK;
}

/*Verify an Alarm property*/
ICalErrorCode verifyAlarmProp(Property* theProp) {

	//check for empty propName
	if (strlen(theProp->propName) <= 1) {
		printf("(empty name)Alarm Prop Name: %s:%s\n", theProp->propName, theProp->propDescr);
		return INV_ALARM;
	}

	//check for too large propName
	if (strlen(theProp->propName) > 200) {
		printf("(too long) Alarm Prop Name: %s:%s\n", theProp->propName, theProp->propDescr);
		return INV_ALARM;
	}

	//check for empty propDescr
	if (strcspn(theProp->propDescr, "\0") == 0) {
		printf("(empty desc) Alarm Prop Name: %s:%s\n", theProp->propName, theProp->propDescr);
		return INV_ALARM;
	}

	//if the property has the name trigger that is invalid
	if(strstr(theProp->propName, "TRIGGER") != NULL) {
		printf("Has a trigger in the proplist\n");
		return INV_ALARM;
	}

	//if the prop name is DTSTART thats invalid
	if(strstr(theProp->propName, "DTSTART") != NULL) {
		printf("has a dt start in the prop list\n");
		return INV_ALARM;
	}


	return OK;
}

/*Verify an alarm*/
ICalErrorCode verifyAlarm(Alarm* theAlarm){

	ICalErrorCode propStatus = OK;
	int hasDur = 0;
	int hasRepeat = 0;
	int hasAtt = 0;

	//check for empty action -- DONE
	if(strlen(theAlarm->action) <= 1) {
		printf("Action empty\n");
		return INV_ALARM;
	}

	//check action length -- DONE
	if(strlen(theAlarm->action) > 200) {
		printf("Action too long\n");
		return INV_ALARM;
	}

	//check for NULL trigger -- DONE
	if(theAlarm->trigger == NULL) {
		printf("trigger NULL\n");
		return INV_ALARM;
	}

	//check for empty tigger -- DONE
	if(strlen(theAlarm->trigger) <= 1) {
		printf("trigger empty: %s\n", theAlarm->trigger);
		return INV_ALARM;
	}

	//check for NUll prop list -- DONE
	if(theAlarm->properties == NULL) {
		printf("propList empty\n");
		return INV_ALARM;
	}

	//make a list iterator for the prop list
	ListIterator propIt = createIterator(theAlarm->properties);
	propIt.current = theAlarm->properties->head;

	while(propIt.current != NULL) {
		Property* toVerify = propIt.current->data;
		if(strstr(toVerify->propName, "DTSTART") != NULL) {
			return INV_ALARM;
		}

		
		//Verify that the property's members are not empty or invalid
		propStatus = verifyAlarmProp(toVerify);

		if(propStatus != OK) {
			return propStatus;
		}

		//Check for duration
		if(strstr(toVerify->propName, "DURATION") != NULL) {
			hasDur++;;
		}
		//check for repeat
		if(strstr(toVerify->propName, "REPEAT") != NULL) {
			hasRepeat++;
		}
		//check for attach
		if (strstr(toVerify->propName, "ATTACH") != NULL) {
			hasAtt++;
		}



		propIt.current = propIt.current->next;
	}

	//all of the duration related invalidations
	if((hasDur == 1 && hasRepeat == 0) || (hasDur == 0 && hasRepeat == 1) || hasDur > 1 || hasRepeat > 1) {
		printf("missing check\n");
		return INV_ALARM;
	}

	//check for more than one attach
	if(hasAtt > 1) {
		printf("too many attaches\n");
		return INV_ALARM;
	}

	return OK;
}

/*Create a calendar based on the fileName
* RETURN: returns a JSON representing the calendar
*/
char* serverCreateCalendar(char* fileName) {
	//create the calendar object
	Calendar* theCal;
	ICalErrorCode theCode = createCalendar(fileName, &theCal);
	
	//if the calendar was made correctly, validate it as well
	if(theCode == OK) {
			theCode = validateCalendar(theCal);
	} else {
	
		return "{}";
	}

	char* JSon;

	//if the cal is good then make its JSon, else delete it and free JSon
	if(theCode == OK) {
		JSon = calendarToJSON(theCal);
		deleteCalendar(theCal);
		return JSon;
	} else {
		deleteCalendar(theCal);
		return "{}";
	}
}

/*Parse a user sent date string and fill DateTime values
*
*/
void parseUserDate(char* date, DateTime* theDate) {	

	//Split string at 8 for date, add date to newDate
	char* dateV = NULL;
	dateV = calloc(1, sizeof(char*) * 9);
	strncpy(dateV, date, 8);
	strncpy(theDate->date, dateV, 9); //conditional jump error here


	//Split after 8 to where T is, add time to newDate
	int Tindex = 0;
	Tindex = strcspn(date, "T");
	char* startTime = NULL;
	startTime = calloc(1, sizeof(char*) * 7);
	memcpy(startTime, date + Tindex + 1, 6);
	strcat(startTime, "\0");
	strcpy(theDate->time, startTime);

	//Add the \0 if its not there
	if(strstr(theDate->date, "\0") == NULL) {
		strcat(theDate->date, "\0");
	}
	
	//Get the UTC value, add it to newDate
	if (strstr(date, "Z") != NULL) {
		theDate->UTC = true;
	} else {
		theDate->UTC = false;
	}


	free(dateV);
	free(startTime);
	
}

char* serverUserCal(char* fileName, char* version, char* prodID, char* UID, char* calDT, char* calST, char* calSum) {
	
	//set the proper file pathway
	
	char* fileN = calloc(1, sizeof(char) * (strlen("uploads/") + strlen(fileName)));
	strcat(fileN, "uploads/");
	strcat(fileN, fileName);

	//Declare the new calendar and its req stuff
	Calendar* newCal;
	Event* newEvt = calloc(1, sizeof(Event));
	DateTime* creation = malloc(sizeof(DateTime));
	DateTime* start = malloc(sizeof(DateTime));
	char* JSon;

	//malloc the new calendar, assign version and prod ID values
	newCal = calloc(1, sizeof(Calendar));
	newCal->version = atof(version);
	strcpy(newCal->prodID, prodID);

	//Initialize the calendar lists
	newCal->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
   	newCal->events = initializeList(&printEvent,&deleteEvent,&compareEvents);

   	//Initialize the event lists
   	newEvt->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
	newEvt->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
   
   	//Parse the date strings for each dateTime
   	parseUserDate(calDT, creation);
   	parseUserDate(calST, start);
   	
   	
   	//Assign the event dates
   	newEvt->creationDateTime = *creation;
   	newEvt->startDateTime = *start;

   	//Assign the other REQ event properties --THIS IS TEMP GET THE USER INPUT AS WELL
   	strcpy(newEvt->UID, UID);

   	//if the calSum string is not empty then make and insert the prop
   	if(strlen(calSum) > 1) {
   		Property* theProp = malloc(sizeof(Property) + (strlen(calSum) + 1) * sizeof(char));
   		strcpy(theProp->propName, "SUMMARY");
   		strcpy(theProp->propDescr, calSum);
   		insertBack(newEvt->properties, theProp);
   	} else {

   	}
   	

   	//add the event to the calendar
   	addEvent(newCal, newEvt);

   	//write the calendar to the file
   	ICalErrorCode writer = writeCalendar(fileN, newCal);
   	//delete the current calendar as we are done with it
   	deleteCalendar(newCal);

   	//if the write is successful, make the calendar
   	if(writer == OK) {
   		ICalErrorCode calErr = createCalendar(fileN, &newCal);
   		//if the cal creation is successful, validate that calendar
   		if(calErr == OK) {
   			calErr = validateCalendar(newCal);
   			//if the validation is successful, make the JSon string, amnd return it
   			if(calErr == OK) {
   				JSon = calendarToJSON(newCal);
   				deleteCalendar(newCal);
   				return JSon;
   			} else {
   				printf("Validation failed!\n");
   				deleteCalendar(newCal);
   				return "{}";
   			}
   		} else {
   			printf("create failed!\n");
   			return "{}";
   		}
   	} else {
   		printf("Write failed!\n");
   		return "{}";
   	}
}

/*Returns a JSON list of all the events within a calendar*/
char* serverEventList(char* fileName) {

	//create a calendar from the fileName
	Calendar* theCal;
	ICalErrorCode theCode = createCalendar(fileName, &theCal);

	//if creation is successful validate the calendar
	if(theCode == OK) {
		theCode = validateCalendar(theCal);
		//if the calendar is valid, make the JSON and return it
		if(theCode == OK) {
			char* JSon;
			JSon = eventListToJSON(theCal->events);
			return JSon;
		} else {
			return "{}";
		}
	} else {
		return "{}";
	}
}

/*Adds a new event to the selected calendar*/
char* serverAddEvt(char* fileName, char* uid, char* startDate, char* startTime, char* summary) {
	//create the calendar
	Calendar* theCal;
	createCalendar(fileName, &theCal);

	//create the new event
	Event* newEvt = calloc(1, sizeof(Event));
	//add the uid to the event
	strcpy(newEvt->UID, uid);

	//create the date members
	DateTime* creation = malloc(sizeof(DateTime));
	DateTime* startT = malloc(sizeof(DateTime));

   	//Initialize the event lists
   	newEvt->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
	newEvt->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
 
   	//Parse the date strings for each dateTime
   	parseUserDate(startDate, creation);
   	parseUserDate(startTime, startT);
   	
   	//Assign the event dates
   	newEvt->creationDateTime = *creation;
   	newEvt->startDateTime = *startT;

   	//create and add the summary property to the event
   	if(strlen(summary) > 1) {
   		Property* theProp = malloc(sizeof(Property) + (strlen(summary) + 1) * sizeof(char));
   		strcpy(theProp->propName, "SUMMARY");
   		strcpy(theProp->propDescr, summary);
   		insertBack(newEvt->properties, theProp);
   	}
   	//add the event to the calendar
   	addEvent(theCal, newEvt);

   	writeCalendar(fileName, theCal);

   	return "OK";
}

/*Convert a property list to a JSON string*/
char* propListtoJSon(Event* theEvent) {
	char* props = calloc(1, sizeof(char) * 1000); //assume no longer than 1000 chars for now

	//add the containing [
	strcpy(props, "[");
	//char* temp = NULL;

	
	ListIterator propIt = createIterator(theEvent->properties); //get the head of the property list
	propIt.current = theEvent->properties->head;
	//iterate over the list of events and make them into JSON strings
	if(theEvent->properties != NULL) {
	
		while(propIt.current != NULL) {
			Property* theProp = propIt.current->data;
			
			char* JSon = calloc(1, sizeof(char) * 500); //assume 500 for now
			strcat(JSon, "{");
			//add the "date"
			strcat(JSon, "\"propName\"");
			//add the colon splitter
			strcat(JSon, ":\"");
			//add the date value
			strcat(JSon, theProp->propName);
			//add the comma seperator
			strcat(JSon, "\",");
			//add the "time"
			strcat(JSon, "\"propDescr\"");
			//add the colon
			strcat(JSon, ":\"");
			//add the time value
			strcat(JSon, theProp->propDescr);
			//add the close }\0
			strcat(JSon, "\"}\0");

			strcat(props, JSon);
			free(JSon);
			if(propIt.current->next != NULL) {
				strcat(props, ",");
			}
			
			free(theProp);
			propIt.current = propIt.current->next;
			
		}
		
	}
	//add the ending ]
	strcat(props, "]");

	//add null temr
	strcat(props, "\0");


	

	return props;
}

/*Finds the event, and converts its property list to a JSON*/
char* serverFindEvent(char* fileName, char* eventNum) {

	int evtNum = atoi(eventNum);

	//create the calendar
	Calendar* theCal;
	ICalErrorCode theCode = createCalendar(fileName, &theCal);

	if(theCode == OK) {
		int i = 1;//this is too keep track of the event we are on
		ListIterator eventIt = createIterator(theCal->events); //make an iterator for the event list
		eventIt.current = theCal->events->head; //get the head of the list
		while (i != evtNum) {
			i++;
			if(eventIt.current->next == NULL) {
				printf("Reached end with finding match!\n");
				return "{}";
			}
			eventIt.current = eventIt.current->next;
		}
		Event* theEvent = eventIt.current->data; //set the event

		char* JSon = propListtoJSon(theEvent);
		//deleteCalendar(theCal);
		//deleteEvent(theEvent);
		
		return JSon;
	} else {
		return "{}";
	}
}

char* alarmListToJSon (Event* theEvent) {
	char* alarms = calloc(1, sizeof(char) * 1000); //assume no longer than 1000 chars for now

	//add the containing [
	strcpy(alarms, "[");
	char* temp = calloc(1, sizeof(char*) * 3);

	
	ListIterator alarmIt = createIterator(theEvent->alarms); //get the head of the property list
	alarmIt.current = theEvent->alarms->head;
	//iterate over the list of events and make them into JSON strings
	if(theEvent->alarms != NULL) {
	
		while(alarmIt.current != NULL) {
			int propVal = 2;
			Alarm* theAlarm = alarmIt.current->data;

			//iterate over the alarm properties
			if(theAlarm->properties != NULL) {
				ListIterator propIt = createIterator(theAlarm->properties);
				propIt.current = theAlarm->properties->head;

				while(propIt.current != NULL) {
					propVal++;

					propIt.current = propIt.current->next;
				}

			}
			
			char* JSon = calloc(1, sizeof(char) * 500); //assume 500 for now
			strcat(JSon, "{");
			//add the "date"
			strcat(JSon, "\"action\"");
			//add the colon splitter
			strcat(JSon, ":\"");
			//add the date value
			strcat(JSon, theAlarm->action);
			//add the comma seperator
			strcat(JSon, "\",");
			//add the "time"
			strcat(JSon, "\"trigger\"");
			//add the colon
			strcat(JSon, ":\"");
			//add the time value
			strcat(JSon, theAlarm->trigger);
			//add the comma seperator
			strcat(JSon, "\",");
			//add the amount of properties in the list
			strcat(JSon, "\"propNum\"");
			//add the colon splitter
			strcat(JSon, ":");
			//add the propVal
			sprintf(temp, "%d", propVal);
			strcat(JSon, temp);
			//add the close }\0
			strcat(JSon, "}\0");

			strcat(alarms, JSon);
			free(JSon);
			if(alarmIt.current->next != NULL) {
				strcat(alarms, ",");
			}
			
			free(theAlarm);
			alarmIt.current = alarmIt.current->next;
			
		}
		
	}
	//add the ending ]
	strcat(alarms, "]");

	//add null temr
	strcat(alarms, "\0");


	free(temp);

	return alarms;
}

/*Returns a JSON string of all the alarms inside of an event*/
char* findAlarmList(char* fileName, char* eventNum) {
	int evtN = atoi(eventNum);

	Calendar* theCal;
	ICalErrorCode theCode = createCalendar(fileName, &theCal);

	if(theCode == OK) {
		int i =1;
		ListIterator eventIt = createIterator(theCal->events);
		eventIt.current = theCal->events->head;

		while (i != evtN) {
			i++;
			if(eventIt.current->next == NULL) {
				printf("Reached end without a match!\n");
				return "{}";
			}
			eventIt.current = eventIt.current->next;
		}
		Event* theEvent = eventIt.current->data;

		char* JSon = alarmListToJSon(theEvent);
		
		return JSon;
	} else  {
		return "{}";
	}
}

/*fucntion for validateing calendars on the server*/
char* validateFiles(char* fileName) {
	Calendar* theCal;
	ICalErrorCode theCode = createCalendar(fileName, &theCal);

	if(theCode != OK) {
		return "Invalid";
	}

	if(validateCalendar(theCal) == OK) {
		return "OK";
	} else {
		return "Invalid";
	}
}





