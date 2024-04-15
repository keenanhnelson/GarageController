#include "EventsAndLogs.hpp"
#include "Arduino.h"
#include "Ticker.h"
#include "GpioSetup.hpp"
#include "WiFiClient.h"
#include "PrivateCodes.hpp"

int NumOfFailedLogIn = 0;
int NumOfFailedAuthentication = 0;
int NumOfSuccessfulLogIn = 0;
int NumOfSuccessfulAuthentication = 0;
int NumOfGarageDoorPresses = 0;
int NumOfEnterRoot = 0;
int NumOfEnterLogin = 0;
int NumOfEnterGarageControlPanel = 0;
int SecSinceOnTime = 0;
int SecWhenLastEventHappened = 0;

String EventsLog = "&lt;br&gt;";//Makes sure the Events log starts on a newline by using &lt;br&gt; == <br>

int garageOpenedFlag = 0;
int garageClosedFlag = 0;
int garageInbetweenFlag = 0;
unsigned long garageStabilityStartTime = 0;
const unsigned long requiredStabiltyTime = 2000;//Require time in milliseconds to wait for stable garage state

Ticker TimeCounter, GarageStateTimer;

#define GARAGE_EVENT_STORAGE 30
GarageEvent garageEvents[GARAGE_EVENT_STORAGE];
int currentNumOfGarageEvents = 0;

#define MSG_LOG_STORAGE 30
String msgLog[MSG_LOG_STORAGE];
int currentNumOfMsgLog = 0;

GarageChangeCause garageChangeCause = CAUSE_MANUAL;
long appLatency = 0;
GarageState_e garageState = GarageInbetween;
GarageState_e garageStatePrevious = GarageInbetween;
GarageState_e garageStateStable = GarageInbetween;

static void addGarageEvent(GarageEvent eventToAdd, GarageEvent *eventList, int previousEventElapsedTime, int *numStored);
static void updateGarageEventDuration(GarageEvent *eventList, int newDuration);
static String getJSON_ValueFromKey(String key, String textToParse);
static void incrementTime();
static void ICACHE_RAM_ATTR handleGarageDoorInbetweenTimerInterrupt();
static void ICACHE_RAM_ATTR handleGarageDoorOpenedInterrupt();
static void ICACHE_RAM_ATTR handleGarageDoorClosedInterrupt();


void eventsAndLogsInit(){
	attachInterrupt(OPENED_DETECT_PIN, handleGarageDoorOpenedInterrupt, FALLING);
	attachInterrupt(CLOSED_DETECT_PIN, handleGarageDoorClosedInterrupt, FALLING);

	//Initialize Ticker
	GarageStateTimer.once(3, handleGarageDoorInbetweenTimerInterrupt);

	TimeCounter.attach(1, incrementTime);
}

void handleEventsAndLogs(){
	//Make sure garage has reached a stable state
	if(garageOpenedFlag){
		garageState = GarageOpened;
		GarageStateTimer.once_ms(1000, handleGarageDoorInbetweenTimerInterrupt);//Require to continually prevent garage inbetween
		garageOpenedFlag = 0;
	}

	if(garageClosedFlag){
		garageState = GarageClosed;
		GarageStateTimer.once_ms(1000, handleGarageDoorInbetweenTimerInterrupt);//Require to continually prevent garage inbetween
		garageClosedFlag = 0;
	}

	if(garageInbetweenFlag){
		if (digitalRead(OPENED_DETECT_PIN) == 1 && digitalRead(CLOSED_DETECT_PIN) == 1) { //check to make sure that door is neither open or closed
			garageState = GarageInbetween;
		}
		GarageStateTimer.once_ms(1000, handleGarageDoorInbetweenTimerInterrupt);
		garageInbetweenFlag = 0;
	}

	//Make sure garage state is staying stable
	if(garageStatePrevious != garageState){
		garageStabilityStartTime = millis();//Resets how long the state has been stable for
	}
	//Check if enough time has elapsed for a stable state
	bool hasEnoughTimeElapsedForStableState = (millis() - garageStabilityStartTime > requiredStabiltyTime);
	//Log garage state if it is in a new stable state, but don't log garageInbetween events
	if(garageStateStable!=garageState && hasEnoughTimeElapsedForStableState && garageState!=GarageInbetween){
		garageStateStable = garageState;
		GarageEvent e = {garageState, getDateTime(), 0, garageChangeCause, appLatency};
		int previousEventElapsedTime = SecSinceOnTime-SecWhenLastEventHappened;
		addGarageEvent(e, garageEvents, previousEventElapsedTime, &currentNumOfGarageEvents);
		garageChangeCause = CAUSE_MANUAL;//Always reset cause back to manual
		SecWhenLastEventHappened = SecSinceOnTime;
	}
	garageStatePrevious = garageState;
}

//There is no direct indication that garage door is inbetween except when neither the open or closed interrupts aren't firing
static void ICACHE_RAM_ATTR handleGarageDoorInbetweenTimerInterrupt() {
	garageInbetweenFlag = 1;
}

//Will constantly fire when open because the open indicator is lit up by pwm signal
static void ICACHE_RAM_ATTR handleGarageDoorOpenedInterrupt() {
	garageOpenedFlag = 1;
}

//Will constantly fire when closed because the close indicator is lit up by pwm signal
static void ICACHE_RAM_ATTR handleGarageDoorClosedInterrupt() {
	garageClosedFlag = 1;
}

static void addGarageEvent(GarageEvent eventToAdd, GarageEvent *eventList, int previousEventElapsedTime, int *numStored){
	//First update the previous event elapsed time
	eventList[0].duration = previousEventElapsedTime;

	//Push all of the events down one to make room for new event at the top
	for(int i=GARAGE_EVENT_STORAGE-2; i>=0; i--){
		eventList[i+1] = eventList[i];
	}

	//Add new event to the top of the list
	garageEvents[0] = eventToAdd;

	//Indicate an increase in the number of stored events
	if(*numStored < GARAGE_EVENT_STORAGE){
		(*numStored)++;
	}

}

static void updateGarageEventDuration(GarageEvent *eventList, int newDuration){
	eventList[0].duration = newDuration;
}

String getGarageEventXMLString(GarageEvent *eventList, int numStored){
	String out = "&lt;br&gt;";
	for(int i=0; i<numStored; i++){
		String e = "";

		if(eventList[i].state == GarageOpened){
			e += String("Opened");
		}
		else if(eventList[i].state == GarageClosed){
			e += String("Closed");
		}

		e += String(" on ") + eventList[i].timestamp;

		e += String(" for ") + eventList[i].duration/3600.0 + " hours";

		if(eventList[i].cause == CAUSE_MANUAL){
			e += String(" by Manual");
		}
		else if(eventList[i].cause == CAUSE_APP){
			e += String(" by App with a latency of ") + eventList[i].appLatency;
		}

		e += String("&lt;br&gt;");

		out += e;
	}
	return out;
}

void addToMsgLog(String *Log, String msgToAdd, int *numStored){
	//Push all of the events down one to make room for new event at the top
	for(int i=MSG_LOG_STORAGE-2; i>=0; i--){
		Log[i+1] = Log[i];
	}

	//Add new event to the top of the list
	Log[0] = msgToAdd;

	//Indicate an increase in the number of stored events
	if(*numStored < MSG_LOG_STORAGE){
		(*numStored)++;
	}
}

String getMsgLogXMLString(String *Log, int numStored){
	String out = "&lt;br&gt;";
	for(int i=0; i<numStored; i++){
		out += Log[i];
		out += String("&lt;br&gt;");
	}
	return out;
}

static String getJSON_ValueFromKey(String key, String textToParse){
	  int index = textToParse.indexOf(key);

	  //Move index forward until colon is reached
	  while(textToParse[index++] != ':' && index < textToParse.length() );

	  String stopChars;
	  //Figure out what the stop character is going to be for the read value
	  while(index < textToParse.length()){
		  if(textToParse[index] == '"'){
			  stopChars = '"';
			  index++;
			  break;
		  }else if((textToParse[index] >= '0' && textToParse[index] <= '9') || textToParse[index] == '-'){
			  stopChars = ", }";
			  break;
		  }else{
			  index++;
		  }
	  }

	  //Start reading the value and saving
	  String out = "";
	  int stopFlag = 0;
	  while(index < textToParse.length()){
		  //Look for all the possible stop characters and break if found
		  for(int i=0; i<stopChars.length(); i++){
			  if(textToParse[index] == stopChars[i]){
				  stopFlag = 1;
				  break;
			  }
		  }
		  //Stop loop if break was found
		  if(stopFlag == 1){
			  break;
		  }
		  out += textToParse[index++];
	  }

	  return out;
}

void getDateTimeAndUnixTime(String *dateTime, unsigned int *unixTime){
	const char* host = "api.timezonedb.com";
	String url = String("/v2.1/get-time-zone?key=") + TIMEZONE_API_KEY + "&format=json&by=zone&zone=America/Los_Angeles&fields=gmtOffset,timestamp,formatted";
	const int httpPort = 80;

	  WiFiClient client;

	  if(!client.connect(host, httpPort)){
		  Serial.println("connection failed");
		  *dateTime = "Failed to get time";
		  *unixTime = 0;
		  return;
	  }

	  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
				   "Host: " + host + "\r\n" +
				   "Connection: close\r\n" +
				   "\r\n"
				  );

	  String line;
	  while (client.connected() || client.available())
	  {
		if (client.available())
		{
		  line += (char)client.read();
		}
	  }
	  client.stop();

	  int gmtOffset = getJSON_ValueFromKey("\"gmtOffset\"", line).toInt();
	  *dateTime = getJSON_ValueFromKey("\"formatted\"", line);
	  *unixTime =  getJSON_ValueFromKey("\"timestamp\"", line).toInt() - gmtOffset;
}

String getDateTime(){
	String dateTime;
	unsigned int unixTime;
	getDateTimeAndUnixTime(&dateTime, &unixTime);
	return dateTime;
}

static void incrementTime(){
	SecSinceOnTime += 1;
	updateGarageEventDuration(garageEvents, SecSinceOnTime-SecWhenLastEventHappened);
}

