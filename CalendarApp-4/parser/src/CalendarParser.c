/*
* Clayton Darlington: 0963286
* CalendarParser.c - Parser for icalendar files with events and actions
* January 23rd 2019
*/
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "LinkedListAPI.h"
#include "helperFunctions.h"
#include "CalendarParser.h"

// ************* List helper functions - MUST be implemented ***************
void deleteProperty(void* toBeDeleted){
	free(toBeDeleted);
}
int compareProperties(const void* first, const void* second){
return 0;
}
char* printProperty(void* toBePrinted){ //Done
	char* c = NULL;

	Property* theProp = (Property*) toBePrinted;

	c = malloc (sizeof(char) * (strlen(theProp->propName) + strlen(theProp->propDescr)) + 1);
	strcat(c, theProp->propName);
	strcat(c, "\n");
	strcat(c, theProp->propDescr);
	strcat(c, "\n");
	free(theProp);
return c;
}

void deleteDate(void* toBeDeleted){
	free(toBeDeleted);
}
int compareDates(const void* first, const void* second){
	DateTime* firstDate = (DateTime*) first;
	DateTime* secondDate = (DateTime*) second;

	if (strcmp(firstDate->date, secondDate->date) != 0) {
		free(firstDate);
		free(secondDate);
		return 0;
	} else if (strcmp(firstDate->time, secondDate->time) != 0) {
		free(firstDate);
		free(secondDate);
		return 0;
	} else if (firstDate->UTC == secondDate->UTC) {
		free(firstDate);
		free(secondDate);
		return 1;
	} else {
		free(firstDate);
		free(secondDate);
		return 1;
	}

}

char* printDate(void* toBePrinted){ //has a random T from something

	char* c = NULL;
	DateTime* theDate = toBePrinted;

	c = malloc(sizeof(char) * (strlen(theDate->date) + strlen(theDate->time) + sizeof(bool)) + 10);

	strcat(c, "Date:");
	strcat(c, theDate->date);
	strcat(c, "\n");
	strcat(c, "Time:");
	strcat(c, theDate->time);
	strcat(c, "\n");
	if (theDate->UTC == true) {
		strcat(c, "T");
	} else {
		strcat(c, "F");
	}

	strcat(c, "\0");
	free(theDate);
	
return c;
}

void deleteAlarm(void* toBeDeleted){
	
		Alarm* tempAlarm;

		tempAlarm = (Alarm*) toBeDeleted;

		if (tempAlarm->properties != NULL) {
			freeList(tempAlarm->properties);
		} else {
			free(tempAlarm->properties);
		}
		if(tempAlarm->trigger != NULL) {
		free(tempAlarm->trigger);
		}

		if(tempAlarm->action != NULL) {
		free(tempAlarm->action);
		}
		
	
}

int compareAlarms(const void* first, const void* second){
return 0;
}
char* printAlarm(void* toBePrinted){ //fix the mallocing of this and how to print props?
	char* c = NULL;
	Alarm* theAlarm = (Alarm*) toBePrinted;

	c = malloc(sizeof(char) * (strlen(theAlarm->trigger) + strlen(theAlarm->action) + sizeof(theAlarm->properties)) + 1);

	strcat(c, "Action:");
	strcat(c, theAlarm->action);
	strcat(c, "\n");
	strcat(c, "Trigger:");
	strcat(c, theAlarm->trigger);
	strcat(c, "\n");
	

	free(theAlarm);

return c;
}

void deleteEvent(void* toBeDeleted) {
	Event* tempEvt;
	
	tempEvt = (Event*) toBeDeleted;

	if(tempEvt->properties != NULL) {
		freeList(tempEvt->properties);
	}

	if(tempEvt->alarms != NULL) {
			
		freeList(tempEvt->alarms);
	}

	free(tempEvt);
}

int compareEvents(const void* first, const void* second){
return 0;
}
char* printEvent(void* toBePrinted){
	Event* theEvent = (Event*) toBePrinted;

	char* UID = malloc(sizeof(char) * 1000);
	strcpy(UID, theEvent->UID);
	printf("UID: %s", UID);
	

	if (theEvent->properties != NULL) {
		//traverse the list and call print prop
	}

	if (theEvent->alarms != NULL) {
		//traverse the list can call print alarm
	}

	char* c = NULL;
	free(UID);
	free(theEvent);
return c;
}
// **************************************************************************


/** Function to create a string representation of a Calendar object.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a string representing the Calndar contents has been created
 *@return a string contaning a humanly readable representation of a Calendar object
 *@param obj - a pointer to a Calendar struct
**/
char* printCalendar(const Calendar* obj){
	char* c = NULL;
	
	if (obj == NULL) {
		return c;
	} else {
		c = malloc(sizeof(char) * (1000 + sizeof(float) + obj->events->length + obj->properties->length));
		char* version = malloc(8 * sizeof(char));
		sprintf(version, "%.2f\n", obj->version);
		strcpy(c, "VERSION:");
		strcat(c, version);
		strcat(c, "\n");
		free(version);
	}
	
    return c;
}


/** Function to "convert" the ICalErrorCode into a humanly redabale string.
 *@return a string contaning a humanly readable representation of the error code by indexing into
          the descr array using rhe error code enum value as an index
 *@param err - an error code
**/
char* printError(ICalErrorCode err){
	char* error = NULL;
	if (err == INV_CAL) {
		error = malloc(strlen("Invalid Calendar!\n") + 1);
		strcpy(error, "Invalid Calendar!\n");
		return error;
	} else if (err == INV_FILE) {
		error = malloc(strlen("Invalid File!\n") + 1);
		strcpy(error, "Invalid File!\n");
		return error;
	} else if (err == INV_VER) {
		error = malloc(strlen("Invalid Version!\n") + 1);
		strcpy(error, "Invalid Version\n");
		return error;
	} else if (err == DUP_VER) {
		error = malloc(strlen("Duplicate Version!\n") + 1);
		strcpy(error, "Duplicate Version\n");
		return error;
	} else if (err == INV_PRODID) {
		error = malloc(strlen("Invalid ProdID!\n") + 1);
		strcpy(error, "Invalid ProdID\n");
		return error;
	} else if (err == DUP_PRODID) {
		error = malloc(strlen("Duplicate ProdID!\n") + 1);
		strcpy(error, "Duplicate ProdID!\n");
		return error;
	} else if (err == INV_EVENT) {
		error = malloc(strlen("Invalid Event!\n") + 1);
		strcpy(error, "Invalid Event!\n");
		return error;
	} else if (err == INV_DT) {
		error = malloc(strlen("Invalid DateTime!\n") + 1);
		strcpy(error, "Invalid DateTime!\n");
		return error;
	} else if (err == INV_ALARM) {
		error = malloc(strlen("Invalid Alarm!\n") + 1);
		strcpy(error, "Invalid Alarm!\n");
		return error;
	} else if (err == WRITE_ERROR) {
		error = malloc(strlen("Write Error!\n") + 1);
		strcpy(error, "Write Error!\n");
		return error;
	} else if (err == OTHER_ERROR) {
		error = malloc(strlen("Mystery Error!\n") + 1);
		strcpy(error, "Mystery Error!\n");
		return error;
	} else {
		error = malloc(strlen("OK\n") + 1);
		strcpy(error, "OK\n");
		return error;
	}
	
}


/** Function to create a Calendar object based on the contents of an iCalendar file.
 *@pre File name cannot be an empty string or NULL.  File name must have the .ics extension.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid calendar has been created, its address was stored in the variable obj, and OK was returned
		or
		An error occurred, the calendar was not created, all temporary memory was freed, obj was set to NULL, and the
		appropriate error code was returned
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param fileName - a string containing the name of the iCalendar file
 *@param a double pointer to a Calendar struct that needs to be allocated
**/
ICalErrorCode createCalendar(char* fileName, Calendar** obj){

	//Values for error checking
	bool foundEnd = false;
	bool foundID = false;
	bool foundVer = false;
	bool hasEvt = false;

	/*Check if the file exists*/
	if(access(fileName, F_OK) == -1) {
		printf("file does not exist\n");
		return INV_FILE;
	}

	/*Check for an empty filename*/
	if(fileName == NULL) {
		printf("empty file name\n");
		return INV_FILE;
	}

	/*malloc a new calendar object*/
	(*obj) = malloc(sizeof(Calendar));
	(*obj)->version = -1.0;
	strcpy((*obj)->prodID, "\0");
	(*obj)->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
   	(*obj)->events = initializeList(&printEvent,&deleteEvent,&compareEvents);

    char* line = calloc(1, sizeof(char) * 300);

    FILE* fp;
    fp = fopen(fileName , "r");

	//check if file is valid
	if (isValidFile(fileName) == false) {
		free(line);
		deleteCalendar((*obj));
		fclose(fp);
		printf("file is not valid\n");
		return INV_FILE;
	}

	/*Check the first line for BEGIN:VCALENDAR if not return the error statement*/
    fgets(line, 1000, fp);

	//checks the Line for CRLF flags
	if(findCRLF(line) == false) {
		printf("%s\n line has no CRLF1", line);
		fclose(fp);
		free(line);
		deleteCalendar((*obj));
		
		return INV_FILE;
	}

	char c = fgetc(fp);
	int count = 0;
			//Check for and deal with a folded line
		while (c ==  ' ' || c == '\t') {

			
			fseek(fp, -1, SEEK_CUR);
			char* temp = NULL;
			temp = calloc(1, sizeof(char) * 1000);
	
			/*get the next line*/
			char* nextLine = calloc(1, sizeof(char*) * 1000);
			fgets(nextLine, 250, fp);

			//realloc the line incase I runout of space here
			if((strlen(line) + strlen(nextLine)) > 1000) {
				line = realloc(line, sizeof(char*) * strlen(line) + strlen(nextLine));
			}
			
			
			//replace my representation of line with the unfoled one
			strcpy(line, unFold(nextLine, line, temp));
			count++;
			strcat(line, "\0");

			//printf("Line:%s", line);

			free(temp);
			free(nextLine);

			c = fgetc(fp);
		}
		
	fseek(fp, -1, SEEK_CUR); //seek back after line folding is finished


	//Check the first line for BEGIN:VCALENDAR
    if (strstr(line, "BEGIN:VCALENDAR") == NULL) {
		free(line);
		fclose(fp);
    	return INV_CAL;
    }
 
	while (fgets(line, 1000, fp) != NULL) {

		//check for the CRLF flag
		if(findCRLF(line) == false) {
			printf("%s\n line has no CRLF1", line);
			fclose(fp);
			free(line);
			deleteCalendar((*obj));
			return INV_FILE;
		}
			

		if (strstr(line, "VERSION") != NULL) {

			foundVer = true;

			char* buffer = NULL;
    		buffer = calloc(1, sizeof(char)* 1000 + 1);

    		int len = 0;
    		len = strcspn(line, ":"); //finds index of first :
    		int endIndex = 0;
    		endIndex = strcspn(line, "\r\n");

    		strncpy(buffer, line + len + 1, endIndex - len);
			
			if(findCRLF(line) == false) {
				free(line);
				free(buffer);
				fclose(fp);
				deleteCalendar((*obj));
				return INV_CAL;
			}

			//Check if its a num or not
			if (atof(buffer) == 0.0) {
				printf("Invalid Version num\n");
				free(line);
				free(buffer);
				fclose(fp);
				deleteCalendar(*obj);
				return INV_VER;
			}

			float version = atof(buffer);

			if (version == (*obj)->version) { //check for dup version
				free(line);
				free(buffer);
				fclose(fp);
				deleteCalendar(*obj);
				return DUP_VER;
			}

			(*obj)->version = version;
			free(buffer);

			if(fgetc(fp) == ';') {
					fseek(fp, -1, SEEK_CUR);
					fgets(line, 1000, fp);
				} else {
					fseek(fp, -1, SEEK_CUR);
			}
    	}else if (strstr(line, "PRODID") != NULL) {

    		foundID = true;

    		char* buffer = calloc(1, sizeof(char)* 1000 + 1);

    		int len = 0;
    		len = strcspn(line, ":"); //finds index of first :
    		int endIndex = 0;
    		endIndex = strcspn(line, "\r\n");

    		strncpy(buffer, line + len + 1, endIndex - len);
			
			/*Checking for an empty prodID*/
			if(strlen(buffer) <= 1) {
				free(line);
				free(buffer);
				fclose(fp);
				deleteCalendar((*obj));
				return INV_PRODID;
			}



			if(findCRLF(line) == false) {
				free(line);
				free(buffer);
				fclose(fp);
				deleteCalendar((*obj));
				return INV_CAL;
			}
			

			//Remove the cr
			int rflag = strcspn(buffer, "\r");
			char* temp = calloc(1, sizeof(char) * strlen(buffer));
			strncpy(temp, buffer, rflag);
			//add the \0
			strcat(temp, "\0");
			strcpy(buffer, temp);
			free(temp);

			if(strcmp(buffer, (*obj)->prodID) == 0) { //unitialized value here
				free(line);
				free(buffer);
				fclose(fp);
				deleteCalendar((*obj));
				return DUP_PRODID;
			}

			strcpy((*obj)->prodID, buffer);

			free(buffer);
    	}else if (strstr(line, "BEGIN:VEVENT") != NULL) {
    		hasEvt = true;
			if(findCRLF(line) == false) {
				free(line);
				fclose(fp);
				deleteCalendar((*obj));
				return INV_CAL;
			}
			Event* newEvent = calloc(1, sizeof(Event));
			strcpy(newEvent->UID, "\0");
			newEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
			newEvent->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
			ICalErrorCode status = parseEvent(fp, newEvent, (*obj)); //parse the event and return the new fp
			
			if (status != OK) {
				free(line);
				fclose(fp);
				deleteCalendar((*obj));
				return status;
			}

    	}else  if (strstr(line, "END:VCALENDAR") != NULL) {
			if(findCRLF(line) == false) {
				free(line);
				fclose(fp);
				deleteCalendar((*obj));
				return INV_CAL;
			}
    		foundEnd = true;

    		if(foundID == false || foundVer == false || hasEvt == false) {
				free(line);
				fclose(fp);
				deleteCalendar(*obj);
				return INV_CAL;
			}
			
			free(line);
			fclose(fp);
			return OK;
    	} else {
			char* buffer = NULL;
			buffer = calloc(1, sizeof(char)* 1000);
			char * const toFree = buffer;
			strcat(buffer, line);
	
		
			char c = fgetc(fp);
			//Check for and deal with a folded line
			while (c ==  ' ' || c == '\t') {
				fseek(fp, -1, SEEK_CUR);
				char* temp = NULL;
				temp = malloc(sizeof(char) * 1000);
		
				/*get the next line*/
				char nextLine[250];
				fgets(nextLine, 250, fp);

				strcpy(buffer, unFold(nextLine, buffer, temp));
		
				free(temp);

				c = fgetc(fp);
			}
		
			fseek(fp, -1, SEEK_CUR); //seek back after line folding is finished
			
			//determine the type of delimiter to use
			int bufferLen = strlen(buffer);
			int semiSplit = strcspn(buffer, ";");
			int colonSplit = strcspn(buffer, ":");
		
			//printf("Buffer: %s", buffer);
			//Determines what delimiter to use
			if (splitter(semiSplit, colonSplit) == 1) {
				
				//Get the property name
				char* name = calloc(1,(sizeof(char) * semiSplit + 1));
				memcpy(name, buffer, semiSplit);
				strcat(name, "\0");
				//printf("CALPROPNAME:: %s\n", name);

				//make a new property
				Property* newProp = NULL;
				newProp = malloc(sizeof(Property) + (bufferLen + 1) * sizeof(char));	
				strcpy(newProp->propName, "\0");
				printf("%s\n", newProp->propName);
				strcpy(newProp->propDescr, "\0");
				memcpy(newProp->propName, name, strlen(name)); 
				//printf("propName: %s\n", newProp->propName);



				//Get the property description
				int descLen = strcspn(buffer, "\r\n");
				char* desc = NULL;
				desc = calloc(1, (sizeof(char) * descLen) + 3); //no malloced right
				memcpy(desc, buffer + semiSplit + 1, bufferLen);
				if (findCRLF(desc) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(buffer, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, buffer, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(buffer, temp);
				free(temp);
				}

				strcpy(newProp->propDescr, desc);
				//printf("propDesc: %s\n", newProp->propDescr);

				insertBack((*obj)->properties, newProp); //something not fully init here
				

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
				//printf("NAME: %s\n", name);

		
				Property* newProp = calloc(1, sizeof(Property) + (bufferLen + 1) * sizeof(char));	
				strcpy(newProp->propName, "\0");
				strcpy(newProp->propDescr, "\0");
				memcpy(newProp->propName, name, strlen(name)); //Properly init a property to fix the mem
				//strcat(newProp->propName, "\0");

				//Get the property description
				int descLen = strcspn(buffer, "\r\n");
				char* desc = calloc(1, (sizeof(char) * descLen) + 3); //no malloced right
				memcpy(desc, buffer + colonSplit + 1, bufferLen);

				if (findCRLF(desc) == true) {
			
				//Remove the crlf
				int crFlag = strcspn(buffer, "\r\n");
				char* temp = calloc(1, sizeof(char) * strlen(buffer));
				strncpy(temp, buffer, crFlag);
				//add the \0
				strcat(temp, "\0");
				strcpy(buffer, temp);
				free(temp);
				}

				strcpy(newProp->propDescr, desc);
				

				insertBack((*obj)->properties, newProp); //something not fully init here

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
	
	if(foundEnd == false) {
		free(line);
		fclose(fp);
		deleteCalendar(*obj);
		return INV_CAL;
	}


	if((*obj)->version == -1.0) {
		free(line);
		fclose(fp);
		deleteCalendar(*obj);
		return INV_CAL;
	}
	

	free(line);
	fclose(fp);

	
	return OK;
}


/** Function to delete all calendar content and free all the memory.
 *@pre Calendar object exists, is not null, and has not been freed
 *@post Calendar object had been freed
 *@return none
 *@param obj - a pointer to a Calendar struct
**/
void deleteCalendar(Calendar* obj){
	//Calendar is empty and can just be freed

		/*call delete list on event list*/
		freeList((void*)obj->events);
		freeList(obj->properties);

		free(obj);
}

/** Function to writing a Calendar object into a file in iCalendar format.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a file representing the
        Calendar contents in iCalendar format has been created
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode writeCalendar(char* fileName, const Calendar* obj){

	if(fileName == NULL) {
		//printf("Null file\n");
		return WRITE_ERROR;
	}

	//if the object is null thats an error
	if(obj == NULL) {
		//printf("Null object\n");
		return WRITE_ERROR;
	}

	ICalErrorCode isValid = validateCalendar(obj);

	//if the calendar is not valid thats an error
	if(isValid != OK) {
		//printf("invalid cal with the file %s\n", fileName);
		return WRITE_ERROR;
	}

	//Check if the ICal object is legit -- this will be a validate calendar call

	//Check if the file has been opened and is empty
	FILE* fp = fopen(fileName, "w+");
	//write the beginning calendar information
	fwrite("BEGIN:VCALENDAR\r\n", sizeof(char) * strlen("BEGIN:VCALENDAR\r\n"), 1, fp);


	//write prodID to file
	char* prodIdent = calloc(1, sizeof(char) * strlen("PRODID:\r\n") + 1000);
	strcat(prodIdent, "PRODID:");
	strcat(prodIdent, obj->prodID);

	if(findCRLF(prodIdent) == false) { //check for the crlf
		strcat(prodIdent, "\r\n");
	} else {
		printf("has CRLF\n");
	}

	strcat(prodIdent, "\0");

	fwrite(prodIdent, sizeof(char) * strlen(prodIdent), 1, fp);
	free(prodIdent);

	//get the version info and write it to a line
	char* buffer = malloc(sizeof(char) * 320); //is 320 right for a float?
	snprintf(buffer, 320, "%.1f", obj->version);
	char* temp = calloc(1, sizeof(char) * 100);
	strcat(temp, "VERSION:");
	strcat(temp, buffer);
	strcat(temp, "\r\n");
	strcat(temp, "\0");
	fwrite(temp, sizeof(char) * strlen(temp), 1, fp);
	free(buffer);
	free(temp);

	//Iterate over the property list
	if (obj->properties != NULL) {
		ListIterator propIt = createIterator(obj->properties);
		propIt.current = obj->properties->head;

		//loop through the prop list
		while (propIt.current != NULL) {
			Property* wProp = propIt.current->data;

			fp = writeProp(fp, wProp);

			propIt.current = propIt.current->next;
		}
	}

	/*Iterate through the calendar event list and write each event to the file*/
	if (obj->events != NULL) {
		//creates an iterator for the event list
		ListIterator eventIt = createIterator(obj->events);
		eventIt.current = obj->events->head;
		
		//Loop through the eventIt list until the end
		while (eventIt.current != NULL) {
			//create a temp event for use
			Event* wEvent = eventIt.current->data;

			//write BEGIN:VEVENT to the file
			fwrite("BEGIN:VEVENT\r\n", sizeof(char) * strlen("BEGIN:VEVENT\r\n"), 1, fp);
			
			
			//write the contents of the the event to a file
			fp = writeEvent(fp, wEvent);

			//write END:VEVENT to the file
			fwrite("END:VEVENT\r\n", sizeof(char) * strlen("END:VEVENT\r\n"), 1, fp);
			//iterate to the next event in the list
			eventIt.current = eventIt.current->next;
			
			
		}
		
	}

	//Add END:VCALENDAR and close the file
	fwrite("END:VCALENDAR\r\n", sizeof(char) * strlen("END:VCALENDAR\r\n"), 1, fp);
	fclose(fp);


	/*Check each string at the end for the CRLF add it if not there!*/

	return OK;
}


/** Function to validating an existing a Calendar object
 *@pre Calendar object exists and is not null
 *@post Calendar has not been modified in any way
 *@return the error code indicating success or the error encountered when validating the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode validateCalendar(const Calendar* obj){

	//for checking alarms
	ICalErrorCode alarmStat = OK;
	ICalErrorCode propStat = OK;
	ICalErrorCode dateStat = OK;

	//check for a NULL obj -- DONE
	if(obj == NULL) {
		//printf("null obj\n");
		return INV_CAL;
	}

	//Check for prodID that is > 1000 -- DONE
	if(strlen(obj->prodID) > 1000){
		//printf("prodID is gay\n");
		return INV_CAL;
	}

	//Check for an empty prodID -- DONE
	if(strlen(obj->prodID) <= 1) {
		//printf("prodId is empty\n");
		return INV_CAL;
	}

	//check for empty/NULL events list -- DONE
	if(obj->events == NULL ) {
		//printf("event  list NULL\n");
		return INV_CAL;
	}

	//check for empty/NULL property list -- DONE
	if(obj->properties == NULL) {
		//printf("prop list NULL\n");
		return INV_CAL;
	}

	//check for empty Events->prop list
	if (obj->events != NULL) {
		//creates an iterator for the event list
		ListIterator eventIt = createIterator(obj->events);
		eventIt.current = obj->events->head;
		
		//Loop through the eventIt list until the end
		while (eventIt.current != NULL) {
			//create a temp event for use
			Event* vEvent = eventIt.current->data;

			//check for empty UID -- DONE
			if(strlen(vEvent->UID) <= 1) {
				//printf("no UID\n");
				return INV_EVENT;
			}

			//check UID length -- DONE
			if(strlen(vEvent->UID) > 1000) {
				//printf("UID to long");
				return INV_EVENT;
			}

			//verify the creationDateTime in an event
			dateStat = verifyDate(vEvent->creationDateTime);

			if (dateStat != OK) {
				return dateStat;
			}

			//verify the startDateTime in an event
			dateStat = verifyDate(vEvent->startDateTime);

			if (dateStat != OK) {
				return dateStat;
			}

			//check for empty event->prop list -- DONE
			if(vEvent->properties == NULL || vEvent->alarms == NULL) {
				//printf("lists null in evt\n");
				return INV_EVENT;
			}

			/*Check all the properties in the list*/
			propStat = verifyEvtPropList(vEvent);

			if(propStat != OK) {
				return propStat;
			}

			//create a list iterator for the alarm list
			ListIterator alarmIt = createIterator(vEvent->alarms);
			alarmIt.current = vEvent->alarms->head;

			//loop over the alarm list
			while (alarmIt.current != NULL) {

				Alarm* vAlarm = alarmIt.current->data;
				alarmStat = verifyAlarm(vAlarm); 

				//if alarm not ok return its alarm code
				if(alarmStat != OK) {
					//printf("alarm not okay\n");
					return alarmStat;
				}

				alarmIt.current = alarmIt.current->next;
			}
			
			//move to next event in the list
			eventIt.current = eventIt.current->next;
			
			
		}	
	}

	//If prop list not NULL, iterate over it checking for validity
	if(obj->properties != NULL) {
		ListIterator propIt = createIterator(obj->properties);
		propIt.current = obj->properties->head;

		int calScaleCount = 0;
		int methodCount = 0;

		//loop over the properties in the list
		while (propIt.current != NULL) {

			//create the temp prop
			Property *toVerify = propIt.current->data;

			//Verify the properties are correct
			propStat = verifyCalProp(toVerify);

			if(propStat != OK) {
				return propStat;
			}

			//check for CALSCALE duplicates
			if(strstr(toVerify->propName, "CALSCALE") != NULL) {
				calScaleCount++;
			}
			//check for METHOD duplicates
			if(strstr(toVerify->propName, "METHOD") != NULL) {
				methodCount++;
			}

			//iterate to next member of the list
			propIt.current = propIt.current->next;

		}

		//duplicates = bad
		if(calScaleCount > 1 || methodCount > 1) {
			return INV_CAL;
		}

	}

	return OK;
}

/** Function to converting a DateTime into a JSON string
 *@pre N/A
 *@post DateTime has not been modified in any way
 *@return A string in JSON format
 *@param prop - a DateTime struct
 **/
char* dtToJSON(DateTime prop) {  // -- DONE --
	

	//Malloc the string to the size required for the passed date time struct -- fix this
	char* JSon = calloc(1, sizeof(char) * 100);

	//add the { brace to the beginning of the JSON
	strcat(JSon, "{");
	//add the "date"
	strcat(JSon, "\"date\"");
	//add the colon splitter
	strcat(JSon, ":\"");
	//add the date value
	strcat(JSon, prop.date);
	//add the comma seperator
	strcat(JSon, "\",");
	//add the "time"
	strcat(JSon, "\"time\"");
	//add the colon
	strcat(JSon, ":\"");
	//add the time value
	strcat(JSon, prop.time);
	//add the comma slitter
	strcat(JSon, "\",");
	//add the UTC
	strcat(JSon, "\"isUTC\"");
	//add the colon split again
	strcat(JSon, ":");

	//determine the utc value
	if(prop.UTC == true) {
		strcat(JSon, "true");
	} else if(prop.UTC == false) {
		strcat(JSon, "false");
	}


	//close the JSON with a }
	strcat(JSon, "}");

	//add the null terminator
	strcat(JSon, "\0");

	//printf("JSON: %s\n", JSon);


	return JSon;

}

/** Function to converting an Event into a JSON string
 *@pre Event is not NULL
 *@post Event has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to an Event struct
 **/
char* eventToJSON(const Event* event) { // -- TEST WITH SOME ALARMS --

	/*If the list is null, return the default string*/
	if(event == NULL) {
		return "{}";
	}
	char* JSon = calloc(1, sizeof(char*) * 500);
	char* DTime = dtToJSON(event->startDateTime);

	//printf("\n %s \n", DTime);

	char* num = calloc(1, sizeof(char*) * 3);
	char* summary = malloc(sizeof(char*) * 500); //assume no longer than 500 chars
	int numProp = 3;
	int numAlrm = 0;
	bool hasSum = false;

	//if the prop list is not null, get the num props
	if(event->properties != NULL) {

		//iterator for the property list
		ListIterator propIt = createIterator(event->properties);
		propIt.current = event->properties->head;

		while(propIt.current != NULL) {
			Property* theProp = propIt.current->data;

			numProp++;

			//check if the summary is there, if yes add it to summary
			if(strstr(theProp->propName, "SUMMARY") != NULL) {
				strcpy(summary, theProp->propDescr);
				hasSum = true;
			}

			propIt.current = propIt.current->next;
		}
	}

	//if the alarm list is not null, get num alarms
	if(event->alarms != NULL) {

		ListIterator alarmIt = createIterator(event->alarms);
		alarmIt.current = event->alarms->head;

		while(alarmIt.current != NULL) {
			numAlrm++;
			alarmIt.current = alarmIt.current->next;
		}
	}

	//add the {
	strcat(JSon, "{");
	//add the "startDT"
	strcat(JSon, "\"startDT\":");
	//add the JSon string for DT start
	strcat(JSon, DTime);
	//add the comma seperator
	strcat(JSon, ",");
	//add numProps tag
	strcat(JSon, "\"numProps\"");
	//add the colon
	strcat(JSon, ":");
	//add the numProps
	sprintf(num, "%d", numProp);
	strcat(JSon, num);
	//add the ,
	strcat(JSon, ",");
	//add numAlarms
	strcat(JSon, "\"numAlarms\"");
	//add the colon
	strcat(JSon, ":");
	//add numAlarms
	sprintf(num, "%d", numAlrm);
	strcat(JSon, num);
	//add the comma
	strcat(JSon, ",");
	//add summary
	strcat(JSon, "\"summary\"");
	//add the colon
	strcat(JSon, ":");
	//check if we found summary
	if(hasSum != true) {
		free(summary);
		strcat(JSon, "\"\"");
	} else if (hasSum == true) {
		strcat(JSon, "\"");
		strcat(JSon, summary);
		strcat(JSon, "\"");
		free(summary);
	}

	//close the JSon string
	strcat(JSon, "}");

	//add the null term
	strcat(JSon, "\0");

	//printf("JSon: %s\n", JSon);

	free(DTime);
	free(num);
	

	return JSon;
}

/** Function to converting an Event list into a JSON string
 *@pre Event list is not NULL
 *@post Event list has not been modified in any way
 *@return A string in JSON format
 *@param eventList - a pointer to an Event list
 **/
char* eventListToJSON(const List* eventList) {

	

	char* events = calloc(1, sizeof(char*) * 1000); //assume no longer than 500 for now

	//add the containing [
	strcpy(events, "[");
	char* temp = NULL;

	//iterate over the list of events and make them into JSON strings
	if(eventList != NULL) {
		ListIterator eventListIt = createIterator((List*) eventList);
		eventListIt.current = eventList->head;
	
		while(eventListIt.current != NULL) {
			temp = eventToJSON(eventListIt.current->data);

			strcat(events, temp);
			free(temp);
			if(eventListIt.current->next != NULL) {
				strcat(events, ",");
			}
			
			eventListIt.current = eventListIt.current->next;
			
		}
		
	}
	//add the ending ]
	strcat(events, "]");

	//add null temr
	strcat(events, "\0");

	return events;
}

/** Function to converting a Calendar into a JSON string
 *@pre Calendar is not NULL
 *@post Calendar has not been modified in any way
 *@return A string in JSON format
 *@param cal - a pointer to a Calendar struct
 **/
char* calendarToJSON(const Calendar* cal) { //-- DONE

	//if cal is null return the base string
	if(cal == NULL) {
		return "{}";
	}

	char* JSon = calloc(1, sizeof(char*) * 300); //assume 300 for now
	char* temp = calloc(1, sizeof(char*) * 3); //assume no larger than 99 version num
	int propVal = 2;
	int evtVal = 0;

	//iterate over the prop list
	if(cal->properties != NULL) {
		ListIterator propIt = createIterator(cal->properties);
		propIt.current = cal->properties->head;

		while(propIt.current != NULL) {
			propVal++;

			propIt.current = propIt.current->next;
		}

	}
	//iterate over the event list
	if(cal->events != NULL) {
		ListIterator eventIt = createIterator(cal->events);
		eventIt.current = cal->events->head;

		while(eventIt.current != NULL) {
			evtVal++;
			eventIt.current = eventIt.current->next;
		}
	}

	strcat(JSon, "{");
	strcat(JSon, "\"version\":");
	sprintf(temp, "%.0f", cal->version);
	strcat(JSon, temp);
	strcat(JSon, ",");
	strcat(JSon, "\"prodID\":\"");
	strcat(JSon, cal->prodID);
	strcat(JSon, "\",");
	strcat(JSon, "\"numProps\":");
	sprintf(temp, "%d", propVal);
	strcat(JSon, temp);
	strcat(JSon, ",");
	strcat(JSon, "\"numEvents\":");
	sprintf(temp, "%d", evtVal);
	strcat(JSon, temp);

	//add the } and the null terminator
	strcat(JSon, "}\0");

	free(temp);


	return JSon;
}

/** Function to converting a JSON string into a Calendar struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Calendar struct
 *@param str - a pointer to a string
 **/
Calendar* JSONtoCalendar(const char* str) {

	if(str == NULL) {
		return NULL;
	}

	//create a calendar and both its lists
	Calendar* cal = calloc(1, sizeof(Calendar));
	cal->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
   	cal->events = initializeList(&printEvent,&deleteEvent,&compareEvents);

   	//split the first chunk of the JSON
   	int comma = strcspn(str, ",");
   	char* temp = calloc(1, sizeof(char*) * 100); //assume no longer than 100 for each member

   	memcpy(temp, str, comma);
   	strcat(temp, "\0");

   	//If temp contains verison, get the version num
   	if(strstr(temp, "version") != NULL) {
   		int colon = strcspn(temp, ":");
   		char* ver = calloc(1, sizeof(char*) * 3); //assuming no version > 99

   		//get the version num
   		memcpy(ver, temp + colon + 1, strlen(temp));

   		
   		float tempF = atof(ver);
   		cal->version = tempF;
   		free(ver);
   		

   	} else {
   		free(temp);
   		return NULL;
   	}
   	free(temp);

   	char* temp2 = calloc(1, sizeof(char*) * 100);

   	//get the 2nd prop in the string -- assumed that its prodID
   	memcpy(temp2, str + comma, strlen(str) - 3);
   	strcat(temp2, "\0");

   	if(strstr(temp2, "prodID") != NULL) {
   		int colon = strcspn(temp2, ":");
   		int brace = strcspn(temp2, "}");
   		char* prod = calloc(1, sizeof(char*) * 100); //assuming no prodID > 100 chars for now

   		//get the prodID out of the string
   		memcpy(prod, temp2 + colon + 2, brace - 1);
   		strcat(prod, "\0");

   		//remove the end quote
   		int quote = strcspn(prod, "\"");
   		char* prodID = calloc(1, sizeof(char*) * strlen(prod));
   		
   		memcpy(prodID, prod, quote);
   		strcat(prodID, "\0");

   		//add to cal
   		strcpy(cal->prodID, prodID);
   		free(prod);
   		free(prodID);
   	} else {
   		free(temp);
   		return NULL;
   	}
   	free(temp2);


	return cal;
}

/** Function to converting a JSON string into an Event struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Event struct
 *@param str - a pointer to a string
 **/
Event* JSONtoEvent(const char* str) { //-- JUST NEEDS ERROR CASE FOR BAD PARSE
	//malloc a new event and its lists
	Event* evt = NULL;
	evt = calloc(1, sizeof(Event));
	evt->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);
	evt->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);

	
	//if str is null return null
	if(str == NULL) {
		return NULL;
	}

	//get the location of the colon
	int colon = strcspn(str, ":");
	//printf("colon: %d\n", colon);

	char* temp = calloc(1, sizeof(char*) * 20); //assume no longer than 20 chars for now

	//copy just past the first " in the UID value
	memcpy(temp, str + colon + 2, strlen(str));
	strcat(temp, "\0");

	//get the location of the "
	int quote = strcspn(temp, "\"");

	//copy from beg to the end of the UID value and add the null term
	memcpy(evt->UID, temp, quote);
	strcat(evt->UID, "\0");

	//printf("%s\n", evt->UID);

	free(temp);

	return evt;
}

/** Function to adding an Event struct to an ixisting Calendar struct
 *@pre arguments are not NULL
 *@post The new event has been added to the calendar's events list
 *@return N/A
 *@param cal - a Calendar struct
 *@param toBeAdded - an Event struct
 **/
void addEvent(Calendar* cal, Event* toBeAdded) { // -- DONE

	//return if null
	if(cal == NULL || toBeAdded == NULL) {
		return;
	}

	insertBack(cal->events, toBeAdded);

}
