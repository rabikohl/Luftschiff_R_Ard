/*
 * server.C
 *
 *  Created on: 31.05.2023
 *      Author: RabiKohl
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
	void set_m_speed(string& response);	// übersetzt den speed_command in einen speed im pwm bereich	
	void set_m_steering(string& response);	// setzt speed in port/starboard speed und addiert/subtrahiert in abhängikeit von steering command um zu lenken
	void set_m_speed_port_starboard();	//setzt in abhängikeit von vorzeichen die motorrichtung, setzt absolutwertals pwm signal
	
	string myResponse(string inputStr);
	int power = 0;
	int powerMotorEinzel= 0;
	int maxpower = 250;
	char lastCommand;
	int con=0;
	bool checkcon = false;
	
	int m_steering_command = 0; 	// -2 bis 2
	int m_speed_starboard = 0; 	//-250 bis 250
	int m_speed_port = 0;		// -250 bis 250
	int m_speed_command = 0;	//-2 bis 2
	int m_speed = 0;
	//int m_climb = 0;
	

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
	gpioSetMode(18,PI_ALT0); 	//Geschwindigkeit Motor 3 (evtl muss ALT5 für PWM)
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

void MyServer::set_m_speed_port_starboard(){
	if(m_speed_port>=0){
		gpioWrite(6, 1);	
	}else{
		gpioWrite(6, 0);
	}
	if(m_speed_starboard>=0){
		gpioWrite(26, 1);
	}else{
		gpioWrite(26, 0);	
	}
	
	gpioPWM(13, abs(m_speed_port));
	gpioPWM(19, abs(m_speed_starboard));	

};

void MyServer::set_m_steering(string &response){
	if(m_speed = 0){ //drehen im stand
		switch(m_steering_command){
			case 0:		response += "AHEAD";
					break;
			case 1:		m_speed_port = 125;
					m_speed_starboard = -125;
					response += "HALF PORT";
					break;
			case 2: 	m_speed_port = 250;
					m_speed_starboard = -250;
					response += "FULL PORT";
					break;
			case -1: 	m_speed_starboard = 125;
					m_speed_port = -125;
					response += "HALF STARBOARD";
					break;
			case -2:	m_speed_starboard = 250;
					m_speed_port = -250;
					response += "FULL STARBOARD";	
					break;
			default: 	response += "steering broken";
					break;
		
		}
	}else{	//lenken in der fahrt
		m_speed_starboard = m_speed;
		m_speed_port = m_speed;
	
		switch(m_steering_command){
				case 0:		response += "AHEAD";
						break;
				case 1:		m_speed_port = m_speed_port - 125;
						response += "HALF PORT";
						break;
				case 2: 	m_speed_port = m_speed_port - 250;
						response += "FULL PORT";
						break;
				case -1: 	m_speed_starboard = m_speed_starboard - 125;
						response += "HALF STARBOARD";
						break;
				case -2:	m_speed_starboard = m_speed_starboard - 250;
						response += "FULL STARBOARD";
						break;
				default: 	response += "steering broken";
						break;
		}
	}
};

void MyServer::set_m_speed(string& response){
	
	switch(m_speed_command){
				case 0: 	m_speed = 0;
						response = "STOP";	
						break;
				case 1:		m_speed = 125;
						response = "HALF AHEAD";
						break;
				case 2:		m_speed = 250;
						response = "FULL AHEAD";
						break;
				case -1:	m_speed = -125;
						response = "HALF ASTERN";
						break;
				case -2:	m_speed = -250;
						response = "FULL ASTERN";
						break;
				default:	response = "speed broken";
						break;
					
		}
};


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
	
			if(m_speed_command<=1){
				m_speed_command = m_speed_command+1;
			}
	
			set_m_speed(response);
			set_m_steering(response);
			set_m_speed_port_starboard();
		
			return response; // response muss an methoden übergeben werden.
		
		}else if(inputStr.compare("s")==0){
			
			if(m_speed_command>=-1){
				m_speed_command = m_speed_command-1;
			}
			
			set_m_speed(response);
			set_m_steering(response);
			set_m_speed_port_starboard();
			
			return response;
		}else if(inputStr.compare("a")==0){
			
			if(m_steering_command>=-1){
				m_steering_command = m_steering_command-1;
			}
			
			set_m_speed(response);
			set_m_steering(response);
			set_m_speed_port_starboard();
		
			return response;
		
		}else if(inputStr.compare("d")==0){
			
			if(m_steering_command<=1){
				m_steering_command = m_steering_command+1;
			}
			
			set_m_speed(response);
			set_m_steering(response);
			set_m_speed_port_starboard();
		
			return response;
	
	}else if(inputStr.compare("hoch")==0){
		/*if(powerMotorEinzel < maxpower){
			powerMotorEinzel=0;
			gpioWrite(15, 1);
			gpioPWM(18, powerMotorEinzel);
		}*/
		powerMotorEinzel=0;
		gpioWrite(15, 1);
		gpioPWM(18, powerMotorEinzel);
		pow=to_string(power);
		response="hoch:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("runter")==0){
		/*if(powerMotorEinzel > 0){
			powerMotorEinzel=0;
			gpioWrite(15, 0);
			gpioPWM(18, powerMotorEinzel);
		}*/
		powerMotorEinzel=0;
		gpioWrite(15, 0);
		gpioPWM(18, powerMotorEinzel);
		pow=to_string(power);
		response="runter:";
		response.append(pow);
		return response;
	}else if(inputStr.compare("stopp")==0){
		//power=0;
		//powerMotorEinzel=0;
		m_speed_command = 0;
		m_steering_command = 0;
		set_m_speed(response);
		set_m_steering(response);
		set_m_speed_port_starboard();
		
		powerMotorEinzel=250;
		gpioPWM(13, m_speed);
		gpioPWM(19, m_speed);
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
