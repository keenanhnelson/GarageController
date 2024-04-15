/*
 * html.hpp
 *
 *  Created on: Oct 7, 2018
 *      Author: KeenanAsus
 */

#ifndef HTML_HPP_
#define HTML_HPP_

#include <Arduino.h>

String LoginHTML = R"=====(
	<html>
		<head>
			<style>
				input{height: 30%; width: 80%; font-size: 300%}
			</style>
		</head>
		<body>
			<form action='/login' method='POST'><br>
				<input type='text' name='USERNAME' placeholder='user name'><br>
				<input type='password' name='PASSWORD' placeholder='password'><br>
				<input type='submit' name='SUBMIT' value='Submit'>
			</form>
			<div id='LoginResult'>
				<!-- placeholder -->
			</div>
		</body>
	</html>
)=====";

String GarageControlPanelHTML = R"=====(
	<html>
		<head>
			<style>
				.GarageButton{height: 30%; width: 100%; font-size: 300%} 
				h1{font-size: 300%}
			</style>
			<script>

				GetXML();
				getMsgLog();

				function GetXML(){
					var xhttp = new XMLHttpRequest();
					xhttp.onreadystatechange = function() {
						if (this.readyState == 4 && this.status == 200) {
						   // Typical action to be performed when the document is ready:
							var XML_Stats = xhttp.responseXML;
							console.log(XML_Stats);
							var FailedLogIn = XML_Stats.getElementsByTagName("FailedLogIn")[0].childNodes[0].nodeValue;
							var FailedAuthentication = XML_Stats.getElementsByTagName("FailedAuthentication")[0].childNodes[0].nodeValue;
							var SuccessfulLogIn = XML_Stats.getElementsByTagName("SuccessfulLogIn")[0].childNodes[0].nodeValue;
							var SuccessfulAuthentication = XML_Stats.getElementsByTagName("SuccessfulAuthentication")[0].childNodes[0].nodeValue;
							var GarageDoorPresses = XML_Stats.getElementsByTagName("GarageDoorPresses")[0].childNodes[0].nodeValue;
							var EnterRoot = XML_Stats.getElementsByTagName("EnterRoot")[0].childNodes[0].nodeValue;
							var EnterLogin = XML_Stats.getElementsByTagName("EnterLogin")[0].childNodes[0].nodeValue;
							var EnterGarageControlPanel = XML_Stats.getElementsByTagName("EnterGarageControlPanel")[0].childNodes[0].nodeValue;
							var Time = XML_Stats.getElementsByTagName("Time")[0].childNodes[0].nodeValue;
							document.getElementById("TestXML").innerHTML = "FailedLogIn="+FailedLogIn+" FailedAuthentication="+FailedAuthentication+" SuccessfulLogIn="+SuccessfulLogIn+" SuccessfulAuthentication="+SuccessfulAuthentication+" GarageDoorPresses="+GarageDoorPresses+" EnterRoot="+EnterRoot+" EnterLogin="+EnterLogin+" EnterGarageControlPanel="+EnterGarageControlPanel+" Time="+Time;

						}
					};
					xhttp.open("GET", "TestXML", true);
					xhttp.send();
				}

				function getMsgLog(){
					var xhttp = new XMLHttpRequest();
					xhttp.onreadystatechange = function() {
						if (this.readyState == 4 && this.status == 200) {
						   // Typical action to be performed when the document is ready:
							var msgLogXML = xhttp.responseXML;
							console.log("msgLogXML = " + msgLogXML);
							var msg = msgLogXML.getElementsByTagName("msgLogXML")[0].childNodes[0].nodeValue;
							console.log("msg = " + msg);
							document.getElementById("msgLog").innerHTML = msg;
						}
					};
					xhttp.open("GET", "GetMsgLog", true);
					xhttp.send();
				}

				function ToggleGarageState(){
					var xhttp = new XMLHttpRequest();
					xhttp.onreadystatechange = function() {
						if (this.readyState == 4 && this.status == 200) {
						   // Typical action to be performed when the document is ready:
						   document.getElementById("GarageState").innerHTML = xhttp.responseText;
						}
					};
					xhttp.open("GET", "ToggleGarageState", true);
					xhttp.send();
				}

				function GetGarageState(){
					var xhttp = new XMLHttpRequest();
					xhttp.onreadystatechange = function() {
						if (this.readyState == 4 && this.status == 200) {
						   // Typical action to be performed when the document is ready:
						   document.getElementById("GarageState").innerHTML = xhttp.responseText;

							//Connection status toggle
							var ConStatus = document.getElementById("ConnectionStatus");
							if (ConStatus.style.display === "none") {
								ConStatus.style.display = "inline";
							} else {
								ConStatus.style.display = "none";
							}
						}
					};
					xhttp.open("GET", "GetGarageState", true);
					xhttp.send();
				}

				setInterval(function() {
					// Call a function repetatively with 2 Second interval
					if(document.hasFocus()){
						GetGarageState();
					}
				}, 500); //Update rate in milliseconds

			</script>
		</head>
		<body>
			<button class='GarageButton' type='button' onclick='ToggleGarageState()'> Open/Close Garage </button><br>
		<h1>
			GarageState = <span id='GarageState'> </span><span id='ConnectionStatus'>.</span>
		</h1><br>
		<div id='TestXML'>0</div><br>
		<div id='msgLog'>0</div>
		</body>
	</html>
)=====";

String DebugInfoHTML = R"=====(

)=====";

#endif /* HTML_HPP_ */
