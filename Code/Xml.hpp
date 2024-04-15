#ifndef XML_HPP_
#define XML_HPP_

#include <Arduino.h>

String DataXML = R"=====(
	<stats>
		<FailedLogIn>0</FailedLogIn>
		<FailedAuthentication>0</FailedAuthentication>
		<SuccessfulLogIn>0</SuccessfulLogIn>
		<SuccessfulAuthentication>0</SuccessfulAuthentication>
		<GarageDoorPresses>0</GarageDoorPresses>
		<EnterRoot>0</EnterRoot>
		<EnterLogin>0</EnterLogin>
		<EnterGarageControlPanel>0</EnterGarageControlPanel>
		<Time>0</Time>
	</stats>
)=====";

String msgLogXML = R"=====(
	<msgLogXML>0</msgLogXML>
)=====";

#endif /* XML_HPP_ */
