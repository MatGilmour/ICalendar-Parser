/*
*@file FileParser.c
*@author Mathieu Gilmour
*@date January 2019
*@brief File parser accepts an input and parses each line into a list
*       of strings
*/

#include "parserHelper.h"

char **parseFile(char *fileName, int *lineCount)
{

    //Null check for file input
    if (fileName == NULL)
    {
        return NULL;
    }

    /*
      - buffer for each character in the file
      - tempString for string creation before being put in a list of strings
      - Properties holds the list of strings
      - fileSize for proper allocation of memory
    */
    char buffer[100000] = "";

    char tempString[100000];
    char **fileList;
    int fileSize = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int counter = 0;
    FILE *fp;

    fp = fopen(fileName, "r");

    if (fp == NULL)
    {
        return NULL;
    }

    //fseek and ftell are used to find the size of the file for memory allocation
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fileSize == 0)
    {
        fclose(fp);
        return NULL;
    }

    //Allocation of memory to the buffer, the list of strings
    fileList = malloc(sizeof(char *) * 100000);

    //passes each character from the file into the buffer
    fread(buffer, sizeof(char), fileSize, fp);

    fclose(fp);

    //Creates strings partitioned by \r\n and adds each string to a list
    for (i = 0; i < fileSize; i++)
    {
        //Case buffer character after \r is null
        if (buffer[i] == '\r' && i == fileSize - 1)
        {
            tempString[0] = '\0';
            freeStringList(fileList, lineCount);
            return NULL;
        }
        //Case buffer character after \r is not newline
        else if (buffer[i] == '\r' && buffer[i + 1] != '\n')
        {
            tempString[0] = '\0';
            freeStringList(fileList, lineCount);
            return NULL;
        }
        //Case buffer character is only a newline and not \r\n
        else if (buffer[i] == '\n')
        {
            tempString[0] = '\0';
            freeStringList(fileList, lineCount);
            return NULL;
        }
        //Case buffer character and one after are \r and \n
        else if (buffer[i] == '\r' && buffer[i + 1] == '\n')
        {
            if (counter > 75)
            {
                tempString[0] = '\0';
                freeStringList(fileList, lineCount);
                return NULL;
            }
            if (buffer[i + 2] == '\t' || buffer[i + 2] == ' ')
            {
                i += 2;
                counter = 0;
                continue;
            }
            if (tempString[0] == ';')
            {
                tempString[0] = '\0';
                i++;
                j = 0;
                counter = 0;
                continue;
            }
            fileList[k] = malloc(sizeof(char) * (strlen(tempString) + 200));
            strcpy(fileList[k++], tempString);
            tempString[0] = '\0';
            j = 0;
            i++;
            *lineCount = *lineCount + 1;
            counter = 0;
        }
        //Case buffer character is at the end of the file
        else if (i == strlen(buffer) - 1)
        {
            tempString[j] = buffer[i];
            tempString[++j] = '\0';
            fileList[k] = malloc(sizeof(char) * (strlen(tempString) + 200));
            strcpy(fileList[k++], tempString);
            tempString[0] = '\0';
            j = 0;
            *lineCount = *lineCount + 1;
        }
        //Default case adds buffer character to the temporary string
        else
        {
            tempString[j] = buffer[i];
            tempString[++j] = '\0';
            counter++;
        }
    }

    return fileList;
}

void freeStringList(char **stringList, int *lineCount)
{
    if (!stringList)
        return;
    for (int i = 0; i < *lineCount; i++)
    {
        if (stringList[i])
        {
            if(stringList[i]) free(stringList[i]);
        }
    }
    free(stringList);
}

ICalErrorCode calParser(int lineCounter, char **stringList)
{
    char *tempTags;
    char *dateSplit;
    int i;
    bool calSwitch = false;
    bool verSwitch = false;
    bool prodidSwitch = false;
    bool eventSwitch = false;
    bool minEvent = false;
    bool uidSwitch = false;
    bool createDTSwitch = false;
    bool startDTSwitch = false;
    bool alarmSwitch = false;
    bool actionSwitch = false;
    bool triggerSwitch = false;
    char buffer[10000];

    if (strcmp(stringList[lineCounter - 1], "END:VCALENDAR") != 0)
    {
        return INV_CAL;
    }

    for (i = 0; i < lineCounter; i++)
    {
        strcpy(buffer, stringList[i]);
        tempTags = strtok(buffer, ":;");

        // *****Case BEGIN*******
        if (strcmp(tempTags, "BEGIN") == 0)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_CAL;
            }

            if (strcmp(tempTags, "VCALENDAR") == 0 && calSwitch == false)
            {
                calSwitch = true;
            }
            else if (strcmp(tempTags, "VCALENDAR") == 0 && calSwitch == true)
            {
                return INV_CAL;
            }

            if (strcmp(tempTags, "VEVENT") == 0 && eventSwitch == false)
            {
                eventSwitch = true;
                minEvent = true;
            }
            else if (strcmp(tempTags, "VEVENT") == 0 && eventSwitch == true)
            {
                return INV_EVENT;
            }

            if (strcmp(tempTags, "VALARM") == 0 && alarmSwitch == false)
            {
                alarmSwitch = true;
            }
            else if (strcmp(tempTags, "VALARM") == 0 && alarmSwitch == true)
            {
                return INV_ALARM;
            }
        }
        // *****Case PRODID*******
        else if (strcmp(tempTags, "PRODID") == 0 && prodidSwitch == false)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_PRODID;
            }

            prodidSwitch = true;
        }
        else if (strcmp(tempTags, "PRODID") == 0 && prodidSwitch == true)
        {
            return DUP_PRODID;
        }
        // *****Case VERSION*******
        else if (strcmp(tempTags, "VERSION") == 0 && verSwitch == false)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_VER;
            }

            verSwitch = true;
        }
        else if (strcmp(tempTags, "VERSION") == 0 && verSwitch == true)
        {
            return DUP_VER;
        }
        // *****Case UID*****
        else if (strcmp(tempTags, "UID") == 0 && uidSwitch == false)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_EVENT;
            }

            uidSwitch = true;
        }
        // *****Case createDateTime*****
        else if (strcmp(tempTags, "DTSTAMP") == 0 && createDTSwitch == false)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_DT;
            }

            if(tempTags[8] != 'T')
            {
                return INV_DT;
            }

            if (strlen(tempTags) > 16)
            {
                return INV_DT;
            }

            dateSplit = strtok(tempTags, "T");

            if(dateSplit == NULL){
                return INV_DT;
            }

            if(strlen(dateSplit) > 9){
                return INV_DT;
            }

            dateSplit = strtok(NULL, "");

            if(dateSplit == NULL){
                return INV_DT;
            }

            if(strlen(dateSplit) > 8){
                return INV_DT;
            }

            createDTSwitch = true;
        }
        // *****Case startDateTime*****
        else if (strcmp(tempTags, "DTSTART") == 0 && startDTSwitch == false)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_DT;
            }

            if(tempTags[8] != 'T')
            {
                return INV_DT;
            }

            if (strlen(tempTags) > 16)
            {
                return INV_DT;
            }

            dateSplit = strtok(tempTags, "T");

            if(dateSplit == NULL){
                return INV_DT;
            }

            if(strlen(dateSplit) > 9){
                return INV_DT;
            }

            dateSplit = strtok(NULL, "");

            if(dateSplit == NULL){
                return INV_DT;
            }

            if(strlen(dateSplit) > 8){
                return INV_DT;
            }

            startDTSwitch = true;
        }
        // *****Case Action*****
        else if (strcmp(tempTags, "ACTION") == 0 && actionSwitch == false)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_ALARM;
            }

            actionSwitch = true;
        }
        else if (strcmp(tempTags, "TRIGGER") == 0 && triggerSwitch == false)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_ALARM;
            }

            triggerSwitch = true;
        }
        // *****Case END*****
        else if (strcmp(tempTags, "END") == 0)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                return INV_CAL;
            }

            if (strcmp(tempTags, "VCALENDAR") == 0 && calSwitch == true)
            {
                calSwitch = false;
            }
            else if (strcmp(tempTags, "VCALENDAR") == 0 && calSwitch == false)
            {
                return INV_CAL;
            }

            if (strcmp(tempTags, "VEVENT") == 0 && eventSwitch == true)
            {
                eventSwitch = false;
            }
            else if (strcmp(tempTags, "VEVENT") == 0 && eventSwitch == false)
            {
                return INV_EVENT;
            }

            if (strcmp(tempTags, "VALARM") == 0 && alarmSwitch == true)
            {
                if (actionSwitch == false)
                {
                    return INV_ALARM;
                }
                if (triggerSwitch == false)
                {
                    return INV_ALARM;
                }

                actionSwitch = false;
                triggerSwitch = false;
                alarmSwitch = false;
            }
            else if (strcmp(tempTags, "VALARM") == 0 && alarmSwitch == false)
            {
                return INV_ALARM;
            }
        }
        else {
            tempTags = strtok(NULL, "");
        
            if(tempTags == NULL && alarmSwitch == true){
                return INV_ALARM;
            }
        }
    }

    if (calSwitch == true || eventSwitch == true || alarmSwitch == true)
    {
        return INV_CAL;
    }

    if (minEvent == false)
    {
        return INV_CAL;
    }

    if (prodidSwitch == false)
    {
        return INV_CAL;
    }
    if (verSwitch == false)
    {
        return INV_CAL;
    }
    if (uidSwitch == false)
    {
        return INV_EVENT;
    }
    if (createDTSwitch == false)
    {
        return INV_EVENT;
    }
    if (startDTSwitch == false)
    {
        return INV_EVENT;
    }

    return OK;
}

Event *createEvent(int firstIndex, int secondIndex, char **parsedList)
{

    Event *tempEvent = malloc(sizeof(Event));
    int i;
    char *tempString;
    char *timeSplit;
    char findEnd[10000];
    bool alarmSwitch = false;
    char buffer[10000];
    int startIndex = 0;
    int endIndex = 0;
    tempEvent->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    tempEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);

    for (i = firstIndex + 1; i <= secondIndex; i++)
    {
        //printf("%s\n", parsedList[i]);
        strcpy(buffer, parsedList[i]);
        tempString = strtok(buffer, ":;");

        if (tempString == NULL || tempString[0] == '\0')
        {
            free(tempEvent);
            return NULL;
        }

        if (strcmp(tempString, "BEGIN") == 0)
        {
            tempString = strtok(NULL, "");

            if (tempString == NULL || tempString[0] == '\0')
            {
                free(tempEvent);
                return NULL;
            }

            if (strcmp(tempString, "VALARM") == 0)
            {
                Alarm *alarm;
                startIndex = i;
                endIndex = i;

                strcpy(findEnd, parsedList[startIndex]);

                while (strcmp(findEnd, "END:VALARM") != 0)
                {
                    endIndex++;
                    strcpy(findEnd, parsedList[endIndex]);
                }

                alarm = createAlarm(startIndex, endIndex, parsedList);

                if (alarm == NULL)
                {
                    free(alarm);
                    free(tempEvent);
                    return NULL;
                }

                insertBack(tempEvent->alarms, (void *)alarm);

                alarmSwitch = true;
                i = endIndex;
            }
        }
        else if (strcmp(tempString, "DTSTAMP") == 0)
        {

            tempString = strtok(NULL, "");

            if (tempString == NULL || tempString[0] == '\0')
            {
                free(tempEvent);
                return NULL;
            }

            timeSplit = strtok(tempString, "T");

            if (timeSplit == NULL)
            {
                free(tempEvent);
                return NULL;
            }

            //timeSplit[strlen(timeSplit)] = '\0';

            strcpy(tempEvent->creationDateTime.date, timeSplit);

            timeSplit = strtok(NULL, "");

            if (timeSplit == NULL)
            {
                free(tempEvent);
                return NULL;
            }

            if (timeSplit[strlen(timeSplit) - 1] == 'Z')
            {
                timeSplit[strlen(timeSplit) - 1] = '\0';
                tempEvent->creationDateTime.UTC = true;
            }
            else
            {
                tempEvent->creationDateTime.UTC = false;
            }

            strcpy(tempEvent->creationDateTime.time, timeSplit);

            timeSplit[0] = '\0';
        }
        else if (strcmp(tempString, "UID") == 0)
        {

            tempString = strtok(NULL, "");

            if (tempString == NULL || tempString[0] == '\0')
            {
                free(tempEvent);
                return NULL;
            }

            strcpy(tempEvent->UID, tempString);
        }
        else if (strcmp(tempString, "DTSTART") == 0)
        {

            tempString = strtok(NULL, "");

            if (tempString == NULL || tempString[0] == '\0')
            {
                free(tempEvent);
                return NULL;
            }

            timeSplit = strtok(tempString, "T");

            if (timeSplit == NULL)
            {
                free(tempEvent);
                return NULL;
            }

            strcpy(tempEvent->startDateTime.date, timeSplit);

            timeSplit = strtok(NULL, "");

            if (timeSplit == NULL)
            {
                free(tempEvent);
                return NULL;
            }

            if (timeSplit[strlen(timeSplit) - 1] == 'Z')
            {
                tempEvent->startDateTime.UTC = true;
                timeSplit[strlen(timeSplit) - 1] = '\0';
            }
            else
            {
                tempEvent->startDateTime.UTC = false;
            }

            strcpy(tempEvent->startDateTime.time, timeSplit);

            timeSplit[0] = '\0';
        }
        else if (strcmp(tempString, "END") == 0)
        {
            tempString = strtok(NULL, "");

            if (tempString == NULL || tempString[0] == '\0')
            {
                free(tempEvent);
                return NULL;
            }

            if (strcmp(tempString, "VALARM") == 0)
            {
                alarmSwitch = false;
            }
        }
        else
        {
            if (alarmSwitch == false)
            {
                char name[10000];
                char desc[10000];
                strcpy(name, tempString);
                
                tempString = strtok(NULL, "");

                if (tempString == NULL || tempString[0] == '\0')
                {
                    free(tempEvent);
                    return NULL;
                }

                strcpy(desc, tempString);
                Property * tempProp = malloc(sizeof(Property) + (sizeof(char) * strlen(desc) + 100));
                strcpy(tempProp->propName, name);
                strcpy(tempProp->propDescr, desc);
                insertBack(tempEvent->properties, (void *)tempProp);
            }
        }
    }
    return tempEvent;
}

Alarm *createAlarm(int firstIndex, int secondIndex, char **parsedList)
{

    Alarm *tempAlarm = malloc(sizeof(Alarm));
    int i;
    char *tempString;
    char buffer[10000];

    tempAlarm->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);

    for (i = firstIndex + 1; i < secondIndex; i++)
    {
        strcpy(buffer, parsedList[i]);
        tempString = strtok(buffer, ":;");

        if (tempString == NULL || tempString[0] == '\0')
        {
            free(tempAlarm);
            return NULL;
        }

        if (strcmp(tempString, "ACTION") == 0)
        {
            tempString = strtok(NULL, "");

            if (tempString == NULL || tempString[0] == '\0')
            {
                free(tempAlarm);
                return NULL;
            }

            //printf("TString:%s\n", tempString);

            strcpy(tempAlarm->action, tempString);
        }
        else if (strcmp(tempString, "TRIGGER") == 0)
        {
            tempString = strtok(NULL, "");

            if (tempString == NULL || tempString[0] == '\0')
            {
                free(tempAlarm);
                return NULL;
            }

            tempAlarm->trigger = malloc(sizeof(char) * strlen(tempString) + 100);

            strcpy(tempAlarm->trigger, tempString);
        }
        else
        {
            char propName[10000];
            char propDesc[10000];
            strcpy(propName, tempString);

            tempString = strtok(NULL, "");

            if (tempString == NULL)
            {
                free(tempAlarm);
                return NULL;
            }
            strcpy(propDesc, tempString);

            Property *tempProperty = malloc(sizeof(Property) + (sizeof(char) * strlen(propDesc) + 100));
            strcpy(tempProperty->propName, propName);
            strcpy(tempProperty->propDescr, propDesc);
            insertBack(tempAlarm->properties, (void *)tempProperty);
        }
    }

    return tempAlarm;
}

ICalErrorCode validateEvent(Event* event){

    if(event == NULL){
        return INV_EVENT;
    }

    ListIterator alarmIter = createIterator(event->alarms);
	void* alarmElem;
    ListIterator propIter = createIterator(event->properties);
	void* propElem;

    //Cannot appear more than once
    bool class = false; bool created = false; bool description = false;
    bool geo = false; bool lastmod = false; bool location = false;
    bool organizer = false; bool priority = false; bool seq = false;
    bool status = false; bool transp = false; bool url = false;
    bool summary = false; bool recurid = false; bool dtend = false; 
    bool duration = false;

    
    if(strlen(event->UID) > 1000){
        return INV_EVENT;
    }
    else if(event->UID[0] == '\0'){
        return INV_EVENT;
    }
    else if(strlen(event->creationDateTime.date) != 8){
        return INV_EVENT;
    }
    else if(event->creationDateTime.date[0] == '\0'){
        return INV_EVENT;
    }
    else if(strlen(event->creationDateTime.time) != 6){
        return INV_EVENT;
    }
    else if(event->creationDateTime.time[0] == '\0'){
        return INV_EVENT;
    }
    else if(strlen(event->startDateTime.date) != 8){
        return INV_EVENT;
    }
    else if(event->startDateTime.date[0] == '\0'){
        return INV_EVENT;
    }
    else if(strlen(event->startDateTime.time) != 6){
        return INV_EVENT;
    }
    else if(event->startDateTime.time[0] == '\0'){
        return INV_EVENT;
    }

    if(event->properties == NULL || event->alarms){
        return INV_EVENT;
    }

    while((propElem = nextElement(&propIter)) != NULL){

        Property* tempProp = (Property*)propElem;
        char propCheck[10000];
        char descCheck[10000];
        strcpy(propCheck, tempProp->propName);
        strcpy(descCheck, tempProp->propDescr);

        if(propCheck[0] == '\0' || strlen(propCheck) > 200){
            return INV_EVENT;
        }
        if(descCheck[0] == '\0' || descCheck == NULL){
            return INV_EVENT;
        }

        if(strcmp(propCheck, "CLASS") == 0 && class == false){
            class = true;
        }
        else if(strcmp(propCheck, "CLASS") == 0 && class == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "CREATED") == 0 && created == false){
            created = true;
        }
        else if(strcmp(propCheck, "CREATED") == 0 && created == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "DESCRIPTION") == 0 && description == false){
            description = true;
        }
        else if(strcmp(propCheck, "DESCRIPTION") == 0 && description == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "GEO") == 0 && geo == false){
            geo = true;
        }
        else if(strcmp(propCheck, "GEO") == 0 && geo == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "LAST-MODIFIED") == 0 && lastmod == false){
            lastmod = true;
        }
        else if(strcmp(propCheck, "LAST-MODIFIED") == 0 &&  lastmod == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "LOCATION") == 0 && location == false){
            location = true;
        }
        else if(strcmp(propCheck, "LOCATION") == 0 && location == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "ORGANIZER") == 0 && organizer == false){
            organizer = true;
        }
        else if(strcmp(propCheck, "ORGANIZER") == 0 && organizer == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "PRIORITY") == 0 && priority == false){
            priority = true;
        }
        else if(strcmp(propCheck, "PRIORITY") == 0 && priority == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "SEQUENCE") == 0 && seq == false){
            seq = true;
        }
        else if(strcmp(propCheck, "SEQUENCE") == 0 && seq == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "STATUS") == 0 && status == false){
            status = true;
        }
        else if(strcmp(propCheck, "STATUS") == 0 && status == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "TRANSP") == 0 && transp == false){
            transp = true;
        }
        else if(strcmp(propCheck, "TRANSP") == 0 && transp == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "URL") == 0 && url == false){
            url = true;
        }
        else if(strcmp(propCheck, "URL") == 0 && url == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "SUMMARY") == 0 && summary == false){
            summary = true;
        }
        else if(strcmp(propCheck, "SUMMARY") == 0 && summary == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "RECURRENCE-ID") == 0 && recurid == false){
            recurid = true;
        }
        else if(strcmp(propCheck, "RECURRENCE-ID") == 0 && recurid == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "DTEND") == 0 && dtend == false){
            dtend = true;
        }
        else if(strcmp(propCheck, "DTEND") == 0 && dtend == true){
            return INV_EVENT;
        }
        else if(strcmp(propCheck, "DURATION") == 0 && duration == false){
            duration = true;
        }
        else if(strcmp(propCheck, "DURATION") == 0 && duration == true){
            return INV_EVENT;
        }

        if(dtend == true && duration == true){
            return INV_EVENT;
        }

        /*
        *
        *   The following is for repeatable properties
        * 
        */
        else if(strcmp(propCheck, "RRULE") == 0){
            continue;
        }
        else if(strcmp(propCheck, "ATTACH") == 0){
            continue;
        }
        else if(strcmp(propCheck, "ATTENDEE") == 0){
            continue;
        }
        else if(strcmp(propCheck, "CATEGORIES") == 0){
            continue;
        }
        else if(strcmp(propCheck, "COMMENT") == 0){
            continue;
        }
        else if(strcmp(propCheck, "CONTACT") == 0){
            continue;
        }
        else if(strcmp(propCheck, "EXDATE") == 0){
            continue;
        }
        else if(strcmp(propCheck, "RELATED-TO") == 0){
            continue;
        }
        else if(strcmp(propCheck, "RESOURCES") == 0){
            continue;
        }
        else if(strcmp(propCheck, "RDATE") == 0){
            continue;
        }
        else{
            return INV_EVENT;
        }
    }

	while((alarmElem = nextElement(&alarmIter)) != NULL){

        Alarm* tempAlarm = (Alarm*)alarmElem;
        ICalErrorCode alarmCode = validateAlarm(tempAlarm);

        if(alarmCode != OK){
            return alarmCode;
        }
    }

    return OK;
}

ICalErrorCode validateAlarm(Alarm* alarm){

    if(alarm == NULL){
        return INV_ALARM;
    }

    ListIterator propIter = createIterator(alarm->properties);
	void* propElem;

    bool duration = false;
    bool repeat = false;
    bool attach = false;

    if(alarm->action[0] == '\0' || strlen(alarm->action) > 200 || strcmp(alarm->action, "AUDIO") != 0){
        return INV_ALARM;
    }
    else if(alarm->trigger == NULL || alarm->trigger[0] == '\0'){
        return INV_ALARM;
    }

    while((propElem = nextElement(&propIter)) != NULL){

        Property* tempProp = (Property*)propElem;
        char propCheck[10000];
        char descCheck[10000];
        strcpy(propCheck, tempProp->propName);
        strcpy(descCheck, tempProp->propDescr);

        if(propCheck[0] == '\0' || strlen(propCheck) > 200){
            return INV_CAL;
        }
        if(descCheck == NULL || descCheck[0] == '\0'){
            return INV_CAL;
        }

        if(strcmp(propCheck, "DURATION") == 0 && duration == false){
            duration = true;
        }
        else if(strcmp(propCheck, "DURATION") == 0 && duration == true){
            return INV_ALARM;
        }
        else if(strcmp(propCheck, "REPEAT") == 0 && repeat == false){
            repeat = true;
        }
        else if(strcmp(propCheck, "REPEAT") == 0 && repeat == true){
            return INV_ALARM;
        }
        else if(strcmp(propCheck, "ATTACH") == 0 && attach == false){
            attach = true;
        }
        else if(strcmp(propCheck, "ATTACH") == 0 && attach == true){
            return INV_ALARM;
        }
    }
    
    if(duration == true && repeat == false){
        return INV_ALARM;
    }
    else if(duration == false && repeat == true){
        return INV_ALARM;
    }

    return OK;
}
