#include "Server.hpp"
#include "PrivateCodes.hpp"
#include "html.hpp"
#include "Xml.hpp"
#include "EventsAndLogs.hpp"
#include "GpioSetup.hpp"
#include "EventsAndLogs.hpp"

ESP8266WebServer server(SERVER_PORT);

static bool is_authentified();
static void handleLogin();
static void handleRoot();
static void handleNotFound();
static void handleGarageControlPanel();
static void handleToggleGarageState();
static void handleTestXML();
static void handleGetMsgLog();
static void handleAndroidOpen();
static void handleGetGarageState();
static void handleGetGarageStateDuration();
static void ReplaceInbetween(String& Input, String FirstMarker, String SecondMarker, String ReplaceInbetweenWith);

void serverInit(){
	server.on("/", handleRoot);
	server.on("/login", handleLogin);
	server.on("/GarageControlPanel", handleGarageControlPanel);
	server.on("/ToggleGarageState", handleToggleGarageState);
	server.on("/GetGarageState", handleGetGarageState);
	server.on("/TestXML", handleTestXML);
	server.on("/AndroidOpenBlueBanana951", handleAndroidOpen);
	server.on("/GetGarageStateDuration", handleGetGarageStateDuration);
	server.on("/GetMsgLog", handleGetMsgLog);

	server.onNotFound(handleNotFound);
	//here the list of headers to be recorded
	const char * headerkeys[] = { "User-Agent", "Cookie" };
	size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
	//ask server to track these headers
	server.collectHeaders(headerkeys, headerkeyssize);
	server.begin();
}

void handleServer(){
	server.handleClient();
}

//Check if header is present and correct
static bool is_authentified() {
	String ClientIP = server.client().remoteIP().toString();
	if (server.hasHeader("Cookie")) {
		String cookie = server.header("Cookie");
		if (cookie.indexOf(SERVER_COOKIE_NAME) != -1 && cookie.indexOf(SERVER_COOKIE_PASSWORD)) {
			server.sendHeader("Set-Cookie", String(SERVER_COOKIE_NAME) + "=" + SERVER_COOKIE_PASSWORD + "; Max-Age=31540000"); //604800 7days
			NumOfSuccessfulAuthentication++;
			return true;
		}
	}
	NumOfFailedAuthentication++;
	return false;
}

//login page, also called for disconnect
static void handleLogin() {
	NumOfEnterLogin++;
	String ClientIP = server.client().remoteIP().toString();


	String msg;
	if (server.hasHeader("Cookie")) {
		Serial.print("Found cookie: ");
		String cookie = server.header("Cookie");
		Serial.println(cookie);
	}
	if (server.hasArg("DISCONNECT")) {
		Serial.println("Disconnection");
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.sendHeader("Set-Cookie", "GarageAccess=0");
		server.send(301);
		return;
	}
	if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
		if (server.arg("USERNAME") == SERVER_USERNAME && server.arg("PASSWORD") == SERVER_PASSWORD) {
			server.sendHeader("Location", "/");
			server.sendHeader("Cache-Control", "private"); //Not sure this does anything
//			server.sendHeader("Set-Cookie", "GarageAccess=BananaByPassActivation951; Max-Age=31540000");
			server.sendHeader("Set-Cookie", String(SERVER_COOKIE_NAME) + "=" + String(SERVER_COOKIE_PASSWORD) + "; Max-Age=31540000");
			server.send(301);
			Serial.println("Log in Successful");
			NumOfSuccessfulLogIn++;
			return;
		}
		NumOfFailedLogIn++;
		msg = "Wrong username/password! try again.";
		ReplaceInbetween(LoginHTML, "<div id='LoginResult'>", "</div>", msg);
		Serial.println("Log in Failed");
	}
	server.send(200, "text/html", LoginHTML);
	ReplaceInbetween(LoginHTML, "<div id='LoginResult'>", "</div>", "<!-- placeholder -->"); //Change back for the next user

}

//root page can be accessed only if authentification is ok
static void handleRoot() {
	NumOfEnterRoot++;
	Serial.println("Enter handleRoot");

	String ClientIP = server.client().remoteIP().toString();
	Serial.printf("ClientIP = %s\r\n", ClientIP.c_str());

	String header;
	if (!is_authentified()) {
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.send(301);
		return;
	}

	server.sendHeader("Location", "/GarageControlPanel");
	server.send(301);
}

//no need authentification
static void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

static void handleGarageControlPanel() {
	NumOfEnterGarageControlPanel++;
	Serial.println("Enter GarageControlPanel");

	String ClientIP = server.client().remoteIP().toString();
	Serial.printf("ClientIP = %s\r\n", ClientIP.c_str());

	if (!is_authentified()) {
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.send(301);
		return;
	}

	server.send(200, "text/html", GarageControlPanelHTML);
}

static void handleToggleGarageState() {
	Serial.println("Enter ToggleGarageState");
	if (!is_authentified()) {
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.send(301);
		return;
	}

	digitalWrite(MOVE_GARAGE_DOOR_PIN, 1);
	delay(500);
	digitalWrite(MOVE_GARAGE_DOOR_PIN, 0);

	String espDateTime = getDateTime();
	String msg = String("Toggled garage state by website on ") + espDateTime;
	addToMsgLog(msgLog, msg, &currentNumOfMsgLog);

	NumOfGarageDoorPresses++;

	server.send(200, "text/plane", "Garage door moving");
}



static void handleTestXML() {
//	Serial.println("Enter TestXML");
	if (!is_authentified()) {
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.send(301);
		return;
	}

	ReplaceInbetween(DataXML, "<FailedLogIn>", "</FailedLogIn>", String(NumOfFailedLogIn));
	ReplaceInbetween(DataXML, "<FailedAuthentication>", "</FailedAuthentication>", String(NumOfFailedAuthentication));
	ReplaceInbetween(DataXML, "<SuccessfulLogIn>", "</SuccessfulLogIn>", String(NumOfSuccessfulLogIn));
	ReplaceInbetween(DataXML, "<SuccessfulAuthentication>", "</SuccessfulAuthentication>", String(NumOfSuccessfulAuthentication));
	ReplaceInbetween(DataXML, "<GarageDoorPresses>", "</GarageDoorPresses>", String(NumOfGarageDoorPresses));
	ReplaceInbetween(DataXML, "<EnterRoot>", "</EnterRoot>", String(NumOfEnterRoot));
	ReplaceInbetween(DataXML, "<EnterLogin>", "</EnterLogin>", String(NumOfEnterLogin));
	ReplaceInbetween(DataXML, "<EnterGarageControlPanel>", "</EnterGarageControlPanel>", String(NumOfEnterGarageControlPanel));
	ReplaceInbetween(DataXML, "<Time>", "</Time>", getGarageEventXMLString(garageEvents, currentNumOfGarageEvents));

//	Serial.println(DataXML);

	server.send(200, "text/xml", DataXML);
}

static void handleGetMsgLog(){
	//Need to check if user is allowed to access this

	ReplaceInbetween(msgLogXML, "<msgLogXML>", "</msgLogXML>", getMsgLogXMLString(msgLog, currentNumOfMsgLog));
	server.send(200, "text/xml", msgLogXML);
}

static void handleAndroidOpen(){
	Serial.println("Enter Android Open");

	if(!server.hasArg("unixTime") || !server.hasArg("pass")){
		addToMsgLog(msgLog, String("Error wrong server args"), &currentNumOfMsgLog);
		server.send(200, "text/plane", "Error");
		return;
	}

	//Make sure that the unixTime variable has some length
	String androidUnixTime = server.arg("unixTime");
	if(androidUnixTime.length() == 0){
		addToMsgLog(msgLog, String("Error server args don't have length"), &currentNumOfMsgLog);
		server.send(200, "text/plane", "Error");
		return;
	}

	//Make sure the password is correct. Probably want to do something more secure than this later
	String androidPass = server.arg("pass");
	if(androidPass != ANDROID_PASSWORD){
		addToMsgLog(msgLog, String("Error android password incorrect"), &currentNumOfMsgLog);
		server.send(200, "text/plane", "Error");
		return;
	}

	String espDateTime;
	unsigned int espUnixTime;
	getDateTimeAndUnixTime(&espDateTime, &espUnixTime);
	appLatency = espUnixTime - androidUnixTime.toInt();

//	int previousEventElapsedTime = SecSinceOnTime-SecWhenLastEventHappened;
	garageChangeCause = CAUSE_APP;
//	GarageEvent gv = {GarageOpened, espDateTime, 0, CAUSE_APP, appLatency};
//	addGarageEvent(gv, garageEvents, previousEventElapsedTime, &currentNumOfGarageEvents);
//	SecWhenLastEventHappened = SecSinceOnTime;

	String msg = String("App Open on ") + espDateTime + String(" with a latency of ") + appLatency + String("s");
	addToMsgLog(msgLog, msg, &currentNumOfMsgLog);

	//Open the garage door if closed
	if (garageState == GarageClosed){
		digitalWrite(MOVE_GARAGE_DOOR_PIN, 1);
		delay(500);
		digitalWrite(MOVE_GARAGE_DOOR_PIN, 0);
	}

	server.send(200, "text/plane", "AndroidOpen");
}

static void handleGetGarageState() {
//	Serial.println("Enter GetGarageState");
	if (!is_authentified()) {
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.send(301);
		return;
	}

	//Update garage state text
	String GarageStateText;
	if (garageState == GarageOpened) {
		GarageStateText = "Opened";
	} else if (garageState == GarageClosed) {
		GarageStateText = "Closed";
	} else if (garageState == GarageInbetween) {
		GarageStateText = "Inbetween";
	}

	server.send(200, "text/plane", GarageStateText);
}

static void handleGetGarageStateDuration(){
//	if (!is_authentified()) {
//		server.sendHeader("Location", "/login");
//		server.sendHeader("Cache-Control", "no-cache");
//		server.send(301);
//		return;
//	}

	String msgOut = "No events";
	if(currentNumOfGarageEvents > 0){
		if(garageEvents[0].state == GarageOpened){
			msgOut = "Opened for ";
		}else if(garageEvents[0].state == GarageClosed){
			msgOut = "Closed for ";
		}else{
			msgOut = "Unknown for ";
		}

		msgOut += String(garageEvents[0].duration/3600.0) + " hours";

		if(garageEvents[0].cause == CAUSE_MANUAL){
			msgOut += String(" by Manual");
		}
		else if(garageEvents[0].cause == CAUSE_APP){
			msgOut += String(" by App with a latency of ") + garageEvents[0].appLatency;
		}
	}
	server.send(200, "text/plane", msgOut);
}

static void ReplaceInbetween(String& Input, String FirstMarker, String SecondMarker, String ReplaceInbetweenWith){
	int Pos1 = Input.indexOf(FirstMarker);
//	Serial.printf("\r\nPos1 = %i\r\n", Pos1);
	int Pos2 = Input.indexOf(SecondMarker);
//	Serial.printf("\r\nPos2 = %i\r\n", Pos2);
	if (Pos1 == -1 || Pos2 == -1) { //Exit function if markers don't exist
		return;
	}
	Pos1 += FirstMarker.length();
//	Serial.printf("\r\nPos1-2 = %i\r\n", Pos1);
	String StringToReplace = Input.substring(Pos1, Pos2);
//	Serial.printf("\r\nStringToReplace = %s\r\n", StringToReplace.c_str());
	Input.replace(FirstMarker+StringToReplace+SecondMarker, FirstMarker+ReplaceInbetweenWith+SecondMarker);
}
