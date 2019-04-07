#include "CalendarParser.h"
#include "parserHelper.h"
#include "LinkedListAPI.h"

void newCal(char* fileName, char* calJSON){

    if(fileName == NULL || calJSON == NULL){
        return;
    }

    Calendar* tempObj = malloc(sizeof(Calendar));
    Event* tempEvent = malloc(sizeof(Event));
    tempObj->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    tempObj->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    tempEvent->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    tempEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);

    char* tempString = malloc(sizeof(char)*10000);
    char* parser;
    Property* tempProp = malloc(sizeof(Property));
    enum ers errCode = OK;

    strcpy(tempString, calJSON);

    parser = strtok(tempString, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, ",");
    if(parser == NULL){
        return;
    }

    tempObj->version = atof(parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempObj->prodID, parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->startDateTime.time, parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->startDateTime.date, parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->UID, parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    if(strcmp(parser, ",") != 0){
        strcpy(tempProp->propName, "SUMMARY");
        strcpy(tempProp->propDescr, parser);
        insertBack(tempEvent->properties, (void*)tempProp);
    }

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->creationDateTime.time, parser);
    
    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->creationDateTime.date, parser);

    addEvent(tempObj, tempEvent);

    errCode = writeCalendar(fileName, tempObj);

    if(errCode != OK){
        return;
    }
    
}

void newEvent(char* fileName, char* eventJSON){

    if(fileName == NULL || eventJSON == NULL){
        return; //"{\"error\":\"filename\"}";
    }

    Event* tempEvent = malloc(sizeof(Event));
    tempEvent->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    tempEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);

    char* tempString = malloc(sizeof(char)*10000);
    char* parser;
    Calendar* tempObj;
    Property* tempProp = malloc(sizeof(Property));
    enum ers errCode = OK;

    errCode = createCalendar(fileName, &tempObj);

    if(errCode != OK){
        printf("Yeete\n");
        return; //"{\"error\":\"cal\"}";
    }

    strcpy(tempString, eventJSON);

    parser = strtok(tempString, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->startDateTime.time, parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->startDateTime.date, parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->UID, parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    if(strcmp(parser, ",") != 0){
        strcpy(tempProp->propName, "SUMMARY");
        strcpy(tempProp->propDescr, parser);
        insertBack(tempEvent->properties, (void*)tempProp);
    }

    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->creationDateTime.time, parser);
    
    parser = strtok(NULL, ":");
    if(parser == NULL){
        return;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        return;
    }

    strcpy(tempEvent->creationDateTime.date, parser);

    addEvent(tempObj, tempEvent);

    errCode = writeCalendar(fileName, tempObj);

    if(errCode != OK){
        return;
    }
}

char* createPropList(char* fileName, int eventNum){

    if(fileName == NULL){
        return "{\"error\":\"filename\"}";
    }

    char* tempString;
    Calendar* tempObj;
    Node* eventNode;
    Event* tempEvent;
    enum ers errCode = OK;

    errCode = createCalendar(fileName, &tempObj);

    if(errCode != OK){
        return "{\"error\":\"cal\"}";
    }

    if(eventNum > tempObj->events->length){
        return "{\"error\":\"true\"}";
    }

    eventNode = tempObj->events->head;
    for(int i = 0; i < eventNum - 1; i++){
        eventNode = eventNode->next;
    }

    tempEvent = (Event*)eventNode->data;

    tempString = propListToJSON(tempEvent->properties);

    return tempString;

}

char* propListToJSON(const List* propList){

    char* tempString = malloc(sizeof(char)*10000);
    Node* tempNode;
    Property* tempProp;

    if(propList == NULL){
        strcpy(tempString, "[]");
        return tempString;
    }

    strcpy(tempString, "[");

    tempNode = propList->head;
    while(tempNode != NULL){

        tempProp = tempNode->data;
        char* propString = propToJSON(tempProp);
        strcat(tempString, propString);
        strcat(tempString, ",");
        tempNode = tempNode->next;
    }

    if(tempString[strlen(tempString) - 1] == ','){
        tempString[strlen(tempString) - 1] = '\0';
    }

    strcat(tempString, "]");
    strcat(tempString, "\0");

    return tempString;
}

char* propToJSON(const Property* property){

    char* tempString = malloc(sizeof(char)*10000);

    if(property == NULL){
        strcpy(tempString, "{}");
        return tempString;
    }

    strcpy(tempString, "{\"name\":\"");
    if(property->propName != NULL)
        strcat(tempString, property->propName);
    strcat(tempString, "\",\"desc\":\"");
    if(property->propDescr != NULL)
        strcat(tempString, property->propDescr);
    strcat(tempString, "\"}");

    return tempString;
}

char* createAlarmList(char* fileName, int eventNum){

    if(fileName == NULL){
        return "{\"error\":\"filename\"}";
    }

    char* tempString;
    Calendar* tempObj;
    Node* eventNode;
    Event* tempEvent;
    enum ers errCode = OK;

    errCode = createCalendar(fileName, &tempObj);

    if(errCode != OK){
        return "{\"error\":\"cal\"}";
    }

    if(eventNum > tempObj->events->length){
        return "{\"error\":\"true\"}";
    }

    eventNode = tempObj->events->head;
    for(int i = 0; i < eventNum - 1; i++){
        eventNode = eventNode->next;
    }

    tempEvent = (Event*)eventNode->data;

    tempString = alarmListToJSON(tempEvent->alarms);

    return tempString;
}
char* alarmListToJSON(const List* alarmList){

    char* tempString = malloc(sizeof(char)*10000);
    Node* tempNode;
    Alarm* tempAlarm;

    if(alarmList == NULL){
        strcpy(tempString, "[]");
        return tempString;
    }

    strcpy(tempString, "[");

    tempNode = alarmList->head;
    while(tempNode != NULL){

        tempAlarm = tempNode->data;
        char* almString = alarmToJSON(tempAlarm);
        strcat(tempString, almString);
        strcat(tempString, ",");
        free(almString);
        tempNode = tempNode->next;
    }

    if(tempString[strlen(tempString) - 1] == ','){
        tempString[strlen(tempString) - 1] = '\0';
    }

    strcat(tempString, "]");
    strcat(tempString, "\0");

    return tempString;

}

char* alarmToJSON(const Alarm* alarm){

    char* tempString = malloc(sizeof(char)*10000);
    char tempNum[50];
    Node* tempProp = alarm->properties->head;
    int propCount = 0;

    if(alarm == NULL){
        strcpy(tempString, "{}");
        return tempString;
    }

    while(tempProp != NULL){
        tempProp = tempProp->next;
        propCount++;
    }

    strcpy(tempString, "{\"action\":\"");
    strcat(tempString, alarm->action);
    strcat(tempString, "\",\"trigger\":\"");
    strcat(tempString, alarm->trigger);
    strcat(tempString, "\",\"propNum\":");
    sprintf(tempNum, "%d", propCount);
    strcat(tempString, tempNum);
    strcat(tempString, "}");

    return tempString;

}

char* createEventList(char* fileName){

    if(fileName == NULL){
        return "{\"error\":\"filename\"}";
    }

    char* tempString;
    Calendar* tempObj;
    enum ers errCode = OK;

    errCode = createCalendar(fileName, &tempObj);

    if(errCode != OK){
        return "{\"error\":\"cal\"}";
    }

    tempString = eventListToJSON(tempObj->events);

    deleteCalendar(tempObj);
    return tempString;
}

char* createJSONCal(char* fileName){

    if(fileName == NULL){
        return "{\"error\":\"filename\"}";
    }

    char* tempString;
    Calendar* tempObj;
    enum ers errCode = OK;

    errCode = createCalendar(fileName, &tempObj);

    if(errCode != OK){
        return "{\"error\":\"cal\"}";
    }
    
    /*errCode = validateCalendar(tempObj);

    if(errCode != OK){
        return "INVyaat";
    }*/

    tempString = calendarToJSON(tempObj);

    deleteCalendar(tempObj);

    return tempString; 
}

ICalErrorCode createCalendar(char *fileName, Calendar **obj)
{
    char **parsedList;
    char **tempList;
    enum ers error;
    int lineCounter = 0;
    int i;
    char *tempTags;
    char findEnd[10000];
    char buffer[10000];
    int firstIndex;
    int secondIndex;
    Calendar *cal = *obj;
    FILE *fp;

    cal = NULL;

    error = OK;

    if (fileName == NULL)
    {
        return INV_FILE;
    }

    fp = fopen(fileName, "r");

    if(fp == NULL){
        return INV_FILE;
    }
    fclose(fp);

    if (fileName[strlen(fileName) - 4] != '.' || fileName[strlen(fileName) - 3] != 'i' ||
        fileName[strlen(fileName) - 2] != 'c' || fileName[strlen(fileName) - 1] != 's')
    {
        return INV_FILE;
    }

    parsedList = parseFile(fileName, &lineCounter);

    if (lineCounter < 4)
    {
        freeStringList(parsedList, &lineCounter);
        return INV_CAL;
    }

    if (parsedList == NULL)
    {
        freeStringList(parsedList, &lineCounter);
        return INV_CAL;
    }

    cal = calloc(1, sizeof(Calendar));

    if (strcmp(parsedList[0], "BEGIN:VCALENDAR") != 0)
    {
        deleteCalendar(cal);
        freeStringList(parsedList, &lineCounter);
        return INV_CAL;
    }

    /*
    This area is reserved for verifying each aspect of the list of strings
    */

    tempList = malloc(sizeof(char *) * lineCounter + 5);

    for (int j = 0; j < lineCounter; j++)
    {
        tempList[j] = malloc(sizeof(char) * strlen(parsedList[j]) + 2);
        strcpy(tempList[j], parsedList[j]);
    }

    error = calParser(lineCounter, tempList);

    freeStringList(tempList, &lineCounter);

    if (error != OK)
    {
        deleteCalendar(cal);
        freeStringList(parsedList, &lineCounter);
        return error;
    }

    cal->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    cal->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);

    for (i = 0; i < lineCounter; i++)
    {
        //printf("%s\n", parsedList[i]);
        strcpy(buffer, parsedList[i]);
        tempTags = strtok(buffer, ":;");

        if (tempTags == NULL)
        {
            deleteCalendar(cal);
            freeStringList(parsedList, &lineCounter);
            return INV_FILE;
        }

        // *****Case BEGIN*******
        if (strcmp(tempTags, "BEGIN") == 0)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                deleteCalendar(cal);
                freeStringList(parsedList, &lineCounter);
                return INV_CAL;
            }
            else if (strcmp(tempTags, "VEVENT") == 0)
            {
                Event *event;
                firstIndex = i;
                secondIndex = i;

                strcpy(findEnd, parsedList[firstIndex]);

                while (strcmp(findEnd, "END:VEVENT") != 0)
                {
                    secondIndex++;
                    strcpy(findEnd, parsedList[secondIndex]);
                }

                event = createEvent(firstIndex, secondIndex, parsedList);

                if (event == NULL)
                {
                    deleteCalendar(cal);
                    freeStringList(parsedList, &lineCounter);
                    return INV_EVENT;
                }

                insertBack(cal->events, (void *)event);
                i = secondIndex;
            }
        }
        // *****Case PRODID*******
        else if (strcmp(tempTags, "PRODID") == 0)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                deleteCalendar(cal);
                freeStringList(parsedList, &lineCounter);
                return INV_PRODID;
            }

            strcpy(cal->prodID, tempTags);
        }
        // *****Case VERSION*******
        else if (strcmp(tempTags, "VERSION") == 0)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                deleteCalendar(cal);
                freeStringList(parsedList, &lineCounter);
                return INV_VER;
            }
            cal->version = atof(tempTags);
        }
        // *****Case END*******
        else if (strcmp(tempTags, "END") == 0)
        {
            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                deleteCalendar(cal);
                freeStringList(parsedList, &lineCounter);
                return INV_CAL;
            }
        }
        // *****Case Properties*******
        else
        {
            char propName[10000];
            char propDesc[10000];
            strcpy(propName, tempTags);

            if(propName == NULL){
                deleteCalendar(cal);
                freeStringList(parsedList, &lineCounter);
                return INV_CAL;
            }

            tempTags = strtok(NULL, "");

            if (tempTags == NULL)
            {
                deleteCalendar(cal);
                freeStringList(parsedList, &lineCounter);
                return INV_CAL;
            }

            strcpy(propDesc, tempTags);

            if(propDesc == NULL){
                deleteCalendar(cal);
                freeStringList(parsedList, &lineCounter);
                return INV_CAL;
            }

            Property *tempProperty = malloc(sizeof(Property) + (sizeof(char) * strlen(propDesc) + 100));
            strcpy(tempProperty->propName, propName);
            strcpy(tempProperty->propDescr, propDesc);
            insertBack(cal->properties, (void *)tempProperty);
        }
    }
    *obj = cal;
    freeStringList(parsedList, &lineCounter);

    return error;
}

void deleteCalendar(Calendar *obj)
{
    if (obj)
    {
        obj->prodID[0] = '\0';
        obj->version = 0;
        if (obj->events != NULL)
        {
            freeList(obj->events);
        }
        if (obj->properties != NULL)
        {
            freeList(obj->properties);
        }
        free(obj);
    }
}

char *printCalendar(const Calendar *obj)
{

    if (obj == NULL)
    {
        return NULL;
    }

    char *tempString = malloc(sizeof(char) * 100000);
    char floatToString[10];
    char *tempEvent;
    char *tempProp;

    strcpy(tempString, "BEGIN:VCALENDAR\n");
    strcat(tempString, "VERSION:");
    sprintf(floatToString, "%.1f", obj->version);
    strcat(tempString, floatToString);
    strcat(tempString, "\n");
    strcat(tempString, "PRODID:");
    strcat(tempString, obj->prodID);

    tempEvent = toString(obj->events);

    if (tempEvent == NULL)
    {
        free(tempString);
        return NULL;
    }

    strcat(tempString, tempEvent);
    free(tempEvent);

    tempProp = toString(obj->properties);
    if (tempProp)
    {
        strcat(tempString, tempProp);
        free(tempProp);
    }

    strcat(tempString, "\nEND:VCALENDAR\n");

    return tempString;
}

char *printError(ICalErrorCode err)
{

    char *errString = malloc(sizeof(char) * 100);

    if (err == OK)
    {
        strcpy(errString, "OK");
    }
    else if (err == INV_FILE)
    {
        strcpy(errString, "INV_FILE");
    }
    else if (err == INV_CAL)
    {
        strcpy(errString, "INV_CAL");
    }
    else if (err == INV_VER)
    {
        strcpy(errString, "INV_VER");
    }
    else if (err == DUP_VER)
    {
        strcpy(errString, "DUP_VER");
    }
    else if (err == INV_PRODID)
    {
        strcpy(errString, "INV_PRODID");
    }
    else if (err == DUP_PRODID)
    {
        strcpy(errString, "DUP_PRODID");
    }
    else if (err == INV_EVENT)
    {
        strcpy(errString, "INV_EVENT");
    }
    else if (err == INV_DT)
    {
        strcpy(errString, "INV_DT");
    }
    else if (err == INV_ALARM)
    {
        strcpy(errString, "INV_ALARM");
    }
    else if (err == WRITE_ERROR)
    {
        strcpy(errString, "WRITE_ERROR");
    }
    else if (err == OTHER_ERROR)
    {
        strcpy(errString, "OTHER_ERROR");
    }
    else
    {
        printf("NO ERROR FOUND?\n");
    }

    return errString;
}

ICalErrorCode writeCalendar(char *fileName, const Calendar *obj)
{
    if(fileName == NULL || obj == NULL){
        return WRITE_ERROR;
    }

    //String concatenation
    FILE *fp;
    char *buffer;
    char *lineString;
    //Temporary objects
    Event *tempEvent;
    Alarm *tempAlarm;
    Property *tempProp;
    //Nodes for iteration
    Node *eventNode;
    Node *alarmNode;
    Node *propNode;

    buffer = malloc(sizeof(char) * 100000);
    lineString = malloc(sizeof(char) * 10000);

    strcpy(buffer, "BEGIN:VCALENDAR\r\n");
    strcat(buffer, "PRODID:");
    strcat(buffer, obj->prodID);
    strcat(buffer, "\r\n");
    strcat(buffer, "VERSION:");
    sprintf(lineString, "%g", obj->version);
    strcat(buffer, lineString);
    strcat(buffer, "\r\n");
    
    propNode = obj->properties->head;
    while(propNode != NULL){
        tempProp = (Property *)propNode->data;
        strcat(buffer, tempProp->propName);
        strcat(buffer, ":");
        strcat(buffer, tempProp->propDescr);
        strcat(buffer, "\r\n");
        propNode = propNode->next;
    }

    eventNode = obj->events->head;
    while (eventNode != NULL)
    {
        tempEvent = (Event *)eventNode->data;
        strcat(buffer, "BEGIN:VEVENT\r\n");
        strcat(buffer, "UID:");
        strcat(buffer, tempEvent->UID);
        strcat(buffer, "\r\n");
        strcat(buffer, "DTSTAMP:");
        strcat(buffer, tempEvent->creationDateTime.date);
        strcat(buffer, "T");
        strcat(buffer, tempEvent->creationDateTime.time);
        if(tempEvent->creationDateTime.UTC == true){
            strcat(buffer, "Z");
        }
        strcat(buffer, "\r\n");
        strcat(buffer, "DTSTART:");
        strcat(buffer, tempEvent->startDateTime.date);
        strcat(buffer, "T");
        strcat(buffer, tempEvent->startDateTime.time);
        if(tempEvent->startDateTime.UTC == true){
            strcat(buffer, "Z");
        }
        strcat(buffer, "\r\n");

        propNode = tempEvent->properties->head;
        while(propNode != NULL){
            tempProp = (Property *)propNode->data;
            strcat(buffer, tempProp->propName);
            strcat(buffer, ":");
            strcat(buffer, tempProp->propDescr);
            strcat(buffer, "\r\n");
            propNode = propNode->next;
        }

        alarmNode = tempEvent->alarms->head;
        while(alarmNode != NULL)
        {
            tempAlarm = (Alarm *)alarmNode->data;
            strcat(buffer, "BEGIN:VALARM\r\n");
            strcat(buffer, "ACTION:");
            strcat(buffer, tempAlarm->action);
            strcat(buffer, "\r\n");
            strcat(buffer, "TRIGGER:");
            strcat(buffer, tempAlarm->trigger);
            strcat(buffer, "\r\n");
            propNode = tempAlarm->properties->head;
            while(propNode != NULL)
            {
                tempProp = (Property *)propNode->data;
                strcat(buffer, tempProp->propName);
                strcat(buffer, ":");
                strcat(buffer, tempProp->propDescr);
                strcat(buffer, "\r\n");
                propNode = propNode->next;
            }
            strcat(buffer, "END:VALARM\r\n");
            alarmNode = alarmNode->next;
        }

        strcat(buffer, "END:VEVENT\r\n");
        eventNode = eventNode->next;
    }

    strcat(buffer, "END:VCALENDAR\r\n");

    fp = fopen(fileName, "w+");

    if(fp == NULL){
        free(buffer);
        free(lineString);
        return WRITE_ERROR;
    }

    fwrite(buffer, 1, strlen(buffer), fp);

    fclose(fp);
    
    free(buffer);
    free(lineString);
    
    return OK;
}

ICalErrorCode validateCalendar(const Calendar *obj)
{
    if(obj == NULL){
        return INV_CAL;
    }

	ListIterator eventIter = createIterator(obj->events);
	void* eventElem;
    ListIterator propIter = createIterator(obj->properties);
	void* propElem;

    bool calscale = false;
    bool method = false;

    if(strlen(obj->prodID) > 1000){
        return INV_CAL;
    }
    else if(obj->prodID == NULL){
        return INV_CAL;
    }
    else if(obj->version <= 0){
        return INV_CAL;
    }
    else if(obj->events == NULL){
        return INV_CAL;
    }
    else if(getLength(obj->events) == 0){
        return INV_CAL;
    }
    else if (obj->properties == NULL || obj->events == NULL) {
        return INV_CAL;
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


        if(strcmp(propCheck, "CALSCALE") == 0 && calscale == false){
            calscale = true;
        }
        else if(strcmp(propCheck, "CALSCALE") == 0 && calscale == true){
            return INV_CAL;
        }
        else if(strcmp(propCheck, "METHOD") == 0 && method == false){

        }
        else if(strcmp(propCheck, "METHOD") == 0 && method == true){
            return INV_CAL;
        }
        else{
            return INV_CAL;
        }
    }
    
    while((eventElem = nextElement(&eventIter)) != NULL){

        Event* tempEvent = (Event*)eventElem;
        ICalErrorCode eventCode = validateEvent(tempEvent);
        
        if(eventCode != OK){
            return eventCode;
        }
    }

    return OK;
}

char* dtToJSON(DateTime prop){

    char* tempString = malloc(sizeof(char)*1000);
    char* utcVal = malloc(sizeof(char)*1000);

    if(prop.UTC == true){
        strcpy(utcVal, "true");
    }
    else{
        strcpy(utcVal, "false");
    }

    strcpy(tempString, "{\"date\":\"");
    strcat(tempString, prop.date);
    strcat(tempString, "\",\"time\":\"");
    strcat(tempString, prop.time);
    strcat(tempString, "\",\"isUTC\":");
    strcat(tempString, utcVal);
    strcat(tempString, "}");
    strcat(tempString, "\0");
    
    free(utcVal);

    return tempString;
}

char* eventToJSON(const Event* event){

    //Json String
    char* tempString = malloc(sizeof(char)* 10000);
    //summary string
    char sumString[10000];
    char locString[1000];
    char orgString[1000];
    //Conversion string for integers
    char* convString = malloc(sizeof(char)* 100);
    char* dtString;
    int alarmCount = 0;
    int propCount = 3;
    Node* propNode;
    Node* alarmNode;
    Property* tempProp;

    sumString[0] = '\0';
    locString[0] = '\0';
    orgString[0] = '\0';

    if(event == NULL){
        strcpy(tempString, "{}");
        free(convString);
        return tempString;
    }

    strcpy(tempString, "{\"startDT\":");
    dtString = dtToJSON(event->startDateTime);
    strcat(tempString, dtString);
    free(dtString);

    propNode = event->properties->head;
    while(propNode != NULL){

        tempProp = (Property*)propNode->data;
        if(strcmp(tempProp->propName, "SUMMARY") == 0){
            strcpy(sumString, tempProp->propDescr);
        }
        else if(strcmp(tempProp->propName, "LOCATION") == 0){
            strcpy(locString, tempProp->propDescr);
        }
        else if(strcmp(tempProp->propName, "ORGANIZER") == 0){
            strcpy(orgString, tempProp->propDescr);
        }
        propCount++;
        propNode = propNode->next;
    }

    alarmNode = event->alarms->head;
    while(alarmNode != NULL){
        alarmCount++;
        alarmNode = alarmNode->next;
    }

    strcat(tempString, ",\"numProps\":");
    sprintf(convString, "%d", propCount);
    strcat(tempString, convString);
    strcat(tempString, ",\"numAlarms\":");
    sprintf(convString, "%d", alarmCount);
    strcat(tempString, convString);
    strcat(tempString, ",\"summary\":");
    if(sumString[0] != '\0'){
        strcat(tempString, "\"");
        strcat(tempString, sumString);
        strcat(tempString, "\",");
    }
    else{
        strcat(tempString, "\"\",");
    }
    strcat(tempString, "\"location\":");
    if(locString[0] != '\0'){
        strcat(tempString, "\"");
        strcat(tempString, locString);
        strcat(tempString, "\",");
    }
    else{
        strcat(tempString, "\"\",");
    }
    strcat(tempString, "\"organizer\":");
    if(orgString[0] != '\0'){
        strcat(tempString, "\"");
        strcat(tempString, orgString);
        strcat(tempString, "\"}");
    }
    else{
        strcat(tempString, "\"\"}");
    }

    strcat(tempString, "\0");

    free(convString);

    return tempString;
}

char* eventListToJSON(const List* eventList){

    char* tempString = malloc(sizeof(char)*10000);
    Node* tempNode;
    Event* tempEvent;

    if(eventList == NULL){
        strcpy(tempString, "[]");
        return tempString;
    }

    strcpy(tempString, "[");

    tempNode = eventList->head;
    while(tempNode != NULL){
        tempEvent = tempNode->data; 
        char* evString;
        evString = eventToJSON(tempEvent);
        strcat(tempString, evString);
        strcat(tempString, ",");
        free(evString);
        tempNode = tempNode->next;
    }

    if(tempString[strlen(tempString) - 1] == ','){
        tempString[strlen(tempString) - 1] = '\0';
    }

    strcat(tempString, "]");
    strcat(tempString, "\0");

    return tempString;
}

char* calendarToJSON(const Calendar* cal){

    char* tempString = malloc(sizeof(char)*10000);
    char* convString = malloc(sizeof(char)*100);
    int propLength = 2;
    int eventLength = 0;

    if(cal == NULL){
        strcpy(tempString, "{}");
    }

    strcpy(tempString, "{\"version\":");
    sprintf(convString, "%g", cal->version);
    strcat(tempString, convString);
    strcat(tempString, ",\"prodID\":\"");
    strcat(tempString, cal->prodID);
    strcat(tempString, "\",\"numProps\":");
    propLength += cal->properties->length;
    sprintf(convString, "%d", propLength);
    strcat(tempString, convString);
    strcat(tempString, ",\"numEvents\":");
    eventLength = cal->events->length;
    sprintf(convString, "%d", eventLength);
    strcat(tempString, convString);
    strcat(tempString, "}");
    strcat(tempString, "\0");

    free(convString);

    return tempString;
}

Calendar* JSONtoCalendar(const char* str){

    if(str == NULL){
        return NULL;
    }

    Calendar* tempCalendar = malloc(sizeof(Calendar));
    char* tempString = malloc(sizeof(char)*10000);
    char* parser;

    strcpy(tempString, str);
    parser = strtok(tempString, ":");
    if(parser == NULL){
        free(tempString);
        deleteCalendar(tempCalendar);
        return NULL;
    }
    parser = strtok(NULL, ",");
    if(parser == NULL){
        free(tempString);
        deleteCalendar(tempCalendar);
        return NULL;
    }

    tempCalendar->version = atof(parser);

    parser = strtok(NULL, ":");
    if(parser == NULL){
        free(tempString);
        deleteCalendar(tempCalendar);
        return NULL;
    }
    parser = strtok(NULL, "\"");
    if(parser == NULL){
        free(tempString);
        deleteCalendar(tempCalendar);
        return NULL;
    }

    strcpy(tempCalendar->prodID, parser);

    printf("%g\n%s\n", tempCalendar->version, tempCalendar->prodID);

    tempCalendar->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    tempCalendar->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);

    free(tempString);

    return tempCalendar;
}

Event* JSONtoEvent(const char* str){

    if(str == NULL){
        return NULL;
    }

    Event* tempEvent = malloc(sizeof(Event));
    char* tempString = malloc(sizeof(char)*1000);
    char* parser;

    strcpy(tempString, str);

    parser = strtok(tempString, ":");
    if(parser == NULL){
        deleteEvent(tempEvent);
        free(tempString);
        return NULL;
    }

    parser = strtok(NULL, "\"");
    if(parser == NULL){
        deleteEvent(tempEvent);
        free(tempString);
        return NULL;
    }

    strcpy(tempEvent->UID, parser);

    printf("%s\n", tempEvent->UID);

    tempEvent->properties = initializeList(&printProperty, &deleteProperty, &compareProperties);
    tempEvent->alarms = initializeList(&printAlarm, &deleteAlarm, &compareAlarms);

    free(tempString);

    return tempEvent;
}

void addEvent(Calendar* cal, Event* toBeAdded){

    if(cal == NULL || toBeAdded == NULL){
        return;
    }

    insertBack(cal->events, toBeAdded);
}

//Helper functions below

void deleteEvent(void *toBeDeleted)
{
    if (toBeDeleted)
    {
        Event *tempEvent;

        tempEvent = (Event *)toBeDeleted;

        tempEvent->UID[0] = '\0';
        tempEvent->creationDateTime.date[0] = '\0';
        tempEvent->creationDateTime.time[0] = '\0';
        tempEvent->startDateTime.date[0] = '\0';
        tempEvent->startDateTime.time[0] = '\0';
        if (tempEvent->alarms)
        {
            freeList(tempEvent->alarms);
        }
        if (tempEvent->properties)
        {
            freeList(tempEvent->properties);
        }
        free(tempEvent);
    }
}

int compareEvents(const void *first, const void *second)
{

    return 0;
}

char *printEvent(void *toBePrinted)
{
    if (toBePrinted == NULL)
    {
        return NULL;
    }

    char *tempString = malloc(sizeof(char) * 10000);
    Event *tempEvent;
    char *tempAlarm;
    char *tempProp;

    tempEvent = (Event *)toBePrinted;

    strcpy(tempString, "BEGIN:VEVENT\n");
    strcat(tempString, "UID:");
    strcat(tempString, tempEvent->UID);
    strcat(tempString, "\n");
    strcat(tempString, "DTSTAMP:");
    strcat(tempString, tempEvent->creationDateTime.date);
    strcat(tempString, "T");
    strcat(tempString, tempEvent->creationDateTime.time);
    strcat(tempString, "\n");
    strcat(tempString, "DTSTART:");
    strcat(tempString, tempEvent->startDateTime.date);
    strcat(tempString, "T");
    strcat(tempString, tempEvent->startDateTime.time);

    tempProp = toString(tempEvent->properties);
    if (tempProp)
    {
        strcat(tempString, tempProp);
        free(tempProp);
    }

    tempAlarm = toString(tempEvent->alarms);
    if (tempAlarm)
    {
        strcat(tempString, tempAlarm);
        free(tempAlarm);
    }

    strcat(tempString, "\nEND:VEVENT");

    return tempString;
}

void deleteAlarm(void *toBeDeleted)
{

    if (toBeDeleted)
    {
        Alarm *tempAlarm;

        tempAlarm = (Alarm *)toBeDeleted;

        tempAlarm->action[0] = '\0';
        if (tempAlarm->trigger != NULL)
        {
            free(tempAlarm->trigger);
        }
        if (tempAlarm->properties != NULL)
        {
            freeList(tempAlarm->properties);
        }
        free(tempAlarm);
    }
}

int compareAlarms(const void *first, const void *second)
{

    return 0;
}

char *printAlarm(void *toBePrinted)
{

    char *tempString = malloc(sizeof(char) * 10000);
    Alarm *tempAlarm;
    char *tempProp;

    tempAlarm = (Alarm *)toBePrinted;

    strcpy(tempString, "BEGIN:VALARM\n");
    strcat(tempString, "ACTION:");
    strcat(tempString, tempAlarm->action);
    strcat(tempString, "\n");
    strcat(tempString, "TRIGGER:");
    strcat(tempString, tempAlarm->trigger);

    tempProp = toString(tempAlarm->properties);
    if (tempProp)
    {
        strcat(tempString, tempProp);
        free(tempProp);
    }

    strcat(tempString, "\nEND:VALARM");

    return tempString;
}

void deleteProperty(void *toBeDeleted)
{

    if (toBeDeleted == NULL)
    {
        return;
    }

    Property *tempProperty = (Property *)toBeDeleted;

    tempProperty->propName[0] = '\0';
    free(tempProperty);
}

int compareProperties(const void *first, const void *second)
{

    return 0;
}

char *printProperty(void *toBePrinted)
{

    char *tempString = malloc(sizeof(char) * 10000);

    Property *tempProp = (Property *)toBePrinted;

    strcpy(tempString, tempProp->propName);
    strcat(tempString, ":");
    strcat(tempString, tempProp->propDescr);

    return tempString;
}

void deleteDate(void *toBeDeleted)
{

    if (toBeDeleted == NULL)
    {
        return;
    }

    DateTime *tempDate;

    tempDate = (DateTime *)toBeDeleted;

    free(tempDate);
}

int compareDates(const void *first, const void *second)
{

    return 0;
}

char *printDate(void *toBePrinted)
{

    char *tempString = malloc(sizeof(char) * 100); /*
    DateTime* tempDate;

    tempDate = (DateTime*)toBePrinted;

    strcpy(tempString, tempDate->date);
    strcat(tempString, "T");
    strcat(tempString, tempDate->time);*/

    strcpy(tempString, "Aie");

    return tempString;
}
