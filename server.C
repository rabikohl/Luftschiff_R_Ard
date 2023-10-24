/*
 * server.C
 *
 *  Created on: 31.05.2023
 *      Author: RabiKohl
 */
/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */
#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime> //contains various functions for manipulating date and time

#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses

#include "SIMPLESOCKET.H"
#include <pigpio.h>
#include <iostream>
#include <thread>

class MyServer : public TCPserver{
public:
	MyServer(int portNmb, int maxSizeData) : TCPserver(portNmb,maxSizeData){
	};
	void checkCon();

private:
	string myResponse(string inputStr);
	int power = 0;
	int powerMotorEinzel= 0;
	int maxpower = 250;
	char lastCommand;
	int con=0;
	bool checkcon = false;

};
using namespace std;
int main(){
	srand(time(nullptr));
	MyServer srv(2022,25);

	gpioTerminate();
	if (gpioInitialise()<0){
	        return 1;
	    }
	thread check(&MyServer::checkCon,&srv);

	gpioSetMode(5,PI_OUTPUT); 	//Modus vom Dual Motor Driver
	gpioSetMode(6,PI_OUTPUT); 	//Richtung Motor 1
	gpioSetMode(15,PI_OUTPUT);	//Richtung Motor 3
	gpioSetMode(26,PI_OUTPUT); 	//Richtung Motor 2
	gpioSetMode(13,PI_ALT0); 	//Geschwindigkeit Motor 1
	gpioSetMode(18,PI_ALT0); 	//Geschwindigkeit Motor 3
	gpioSetMode(19,PI_ALT5);	//Geschwindigkeit Motor 2
	gpioSetPWMrange(13,255);
	gpioSetPWMrange(18,255);
	gpioSetPWMrange(19,255);
	gpioWrite(5,1);
	gpioPWM(13,0);
	gpioPWM(18,0);
	gpioPWM(19,0);

	srv.run();
	check.detach();
	gpioTerminate();

}


string MyServer::myResponse(string inputStr){
	string response,pow;

	if(inputStr.compare("run")==0){
		con=1;
		return string("run");
	}
	if(inputStr.compare("connected")==0){
		checkcon=true;
		return string("conOk");
	}

	if(inputStr.compare("w")==0){
		if(lastCommand !='w'){
			lastCommand = 'w';
			power=250;  // än
		}
		if(power < maxpower){
			//power=power+5;
			power=power-5;
			gpioWrite(6, 1);
			gpioWrite(26, 1);
			gpioPWM(13, power);
			gpioPWM(19, power);
		}
		pow=to_string(power);
		response="vorwärts:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("s")==0){
		if(lastCommand !='s'){
			lastCommand = 's';
		}
		/*if(power > 0){
			power=power-5;
			gpioPWM(13, power);
			gpioPWM(19, power);
		}*/
		gpioWrite(6, 0);	//bremsen
		gpioWrite(26, 0);
		gpioPWM(13, 125);
		gpioPWM(19, 125);
		
		
		pow=to_string(power);
		response="bremsen:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("a")==0){
		if(lastCommand !='a'){
			lastCommand = 'a';
			power=0;
			gpioWrite(6, 0);
			gpioWrite(26, 1);
		}
		if(power < maxpower){
			power=power+5;
			gpioPWM(13, power);
			gpioPWM(19, power);
		}
		pow=to_string(power);
		response="links:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("d")==0){
		if(lastCommand !='d'){
			lastCommand = 'd';
			power=0;
			gpioWrite(6, 1);
			gpioWrite(26, 0);
		}
		if(power < maxpower){
			power=power+5;
			gpioPWM(13, power);
			gpioPWM(19, power);
		}
		pow=to_string(power);
		response="rechts:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("hoch")==0){
		if(powerMotorEinzel < maxpower){
			powerMotorEinzel=powerMotorEinzel+5;
			gpioWrite(15, 1);
			gpioPWM(18, powerMotorEinzel);
		}
		pow=to_string(power);
		response="hoch:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("runter")==0){
		if(powerMotorEinzel > 0){
			powerMotorEinzel=powerMotorEinzel-5;
			gpioWrite(15, 0);
			gpioPWM(18, powerMotorEinzel);
		}
		pow=to_string(power);
		response="runter:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("stopp")==0){
		//power=0;
		//powerMotorEinzel=0;
		power=250;
		powerMotorEinzel=250;
		gpioPWM(13, power);
		gpioPWM(19, power);
		gpioPWM(18, powerMotorEinzel);

		return string("stopp");
	}
	return string("unknown command");

}


void MyServer::checkCon(){
	while(true){
		if(checkcon==true){
			break;
		}
	}
	while(checkcon){
		con=0;
		sleep(2);
		if(con==0){
			//checkcon=false;
			cout<<"Verbindung verloren!\n";
			power=0;
			powerMotorEinzel=0;
			gpioPWM(13, power);
			gpioPWM(19, power);
			gpioPWM(18, powerMotorEinzel);
		}
	}
}
