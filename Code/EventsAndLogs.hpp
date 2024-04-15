#ifndef EVENTANDLOGS_HPP_
#define EVENTANDLOGS_HPP_

#include "Arduino.h"

typedef enum {
	GarageOpened, GarageClosed, GarageInbetween
} GarageState_e;

typedef enum{
	CAUSE_MANUAL, CAUSE_APP
}GarageChangeCause;

typedef struct{
	GarageState_e state;//GarageState
	String timestamp;//Date and time when first in this state (yyyy-mm-dd hh:mm:ss)
	int duration;//How long it is/was in this state (in seconds)
	GarageChangeCause cause;//What caused this state. Manual or app
	long appLatency;//How long the app command took to get to the garage door
}GarageEvent;

extern int NumOfFailedLogIn;
extern int NumOfFailedAuthentication;
extern int NumOfSuccessfulLogIn;
extern int NumOfSuccessfulAuthentication;
extern int NumOfGarageDoorPresses;
extern int NumOfEnterRoot;
extern int NumOfEnterLogin;
extern int NumOfEnterGarageControlPanel;
extern int SecSinceOnTime;
extern int SecWhenLastEventHappened;

extern GarageEvent garageEvents[];
extern int currentNumOfGarageEvents;

extern String msgLog[];
extern int currentNumOfMsgLog;

extern long appLatency;

extern GarageChangeCause garageChangeCause;

extern GarageState_e garageState;

void eventsAndLogsInit();
void getDateTimeAndUnixTime(String *dateTime, unsigned int *unixTime);
String getDateTime();
void addToMsgLog(String *Log, String msgToAdd, int *numStored);
String getMsgLogXMLString(String *Log, int numStored);
String getGarageEventXMLString(GarageEvent *eventList, int numStored);
void handleEventsAndLogs();

#endif /* LOGS_HPP_ */
