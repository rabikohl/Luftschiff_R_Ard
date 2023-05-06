/*
 * server_new.C
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

class MyServer : public TCPserver{
public:
	MyServer(int portNmb, int maxSizeData) : TCPserver(portNmb,maxSizeData){
	};

protected:
	string myResponse(string inputStr);
	int power = 0;
	char lastCommand;

};
using namespace std;
int main(){
	srand(time(nullptr));
	MyServer srv(2022,25);

	gpioTerminate();
	if (gpioInitialise()<0){
	        return 1;
	    }
                                        // https://www.raspberry-pi-geek.de/ausgaben/rpg/2020/04/grundlagen-der-pulsweitenmodulation/
	gpioSetMode(5, PI_OUTPUT);          // unten Motor
	gpioSetMode(18,PI_ALT1);     // PWM Channel 0 (fct5)       
	gpioSetPWMrange(18, 255);

    gpioSetMode(6, PI_OUTPUT);          // links Motor
	gpioSetMode(13,PI_ALT0);            //PWM channel 1 (fct 4)
	gpioSetPWMrange(13, 255);

    gpioSetMode(26, PI_OUTPUT);          //rechts Motor
	gpioSetMode(19,PI_ALT5);             // PWM Channel 1(fct 2)
	gpioSetPWMrange(19, 255);

                                            // gedanken zum PWM CHAnnel  --> die beiden hinteren Motoren müsste man auch mit einem PWM Channel stuern lassen können , es gibt nur das Problem das der Motor zum abschalten
                                            // dann nicht runtergedreht wird sonder naus gemacht werden muss bzw. dann auf Last wieder angeschaltet werden müsste aus der ruhe auf den vorherigen Wert. vielleicht könnte man den beide zuerst etwas drosseln  und den einen kurz zum stehen bleiben bringen und dann wieder anfangen ihn rückwärts laufen zulassen
         gpioSetMode(21, PI_OUTPUT);                                   // es wären damit aber keine Sanften Kurven zum fliegen möglich
	gpiodwrite(21,1);			// Mode Ansteurerung Dual Motorshield --> wird auf 1 gelegt so das Signal an ist
	srv.run();
	gpioTerminate();

}


string MyServer::myResponse(string inputStr){
	string response;

	if(inputStr.compare("w")==0){
		if(lastCommand == 'w'){
			if(power < 10){
				power++;
			}
		}else{
			power = 1;
			lastCommand = 'w';
		}
		cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;

		gpioWrite(6, 1);
        gpioWrite(26, 1);

		gpioPWM(13, 5*power);
        gpioPWM(19, 5*power);

		return string("vorwärts");
	}else if(inputStr.compare("s")==0){
		if(lastCommand == 's'){
			if(power < 10){
				power++;
			}
			}else{
				power = 1;
				lastCommand = 's';
			}
			cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;
			gpioWrite(6, 0);                    
            gpioWrite(26, 0);                                                                 // wieso hier auf 0 sollte man den niht runter regeln ?
			
            gpioPWM(13, 5*power);
            gpioPWM(19, 5*power);

			return string("bremsen");


    }else if(inputStr.compare("a")==0){                           // links Kurve 
		if(lastCommand == 'a'){
			if(power < 10){
				power++;
			}
			}else{
				power = 1;
				lastCommand = 'a';
			}
			cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;
			gpioWrite(6, 0);                    
            gpioWrite(26, 1);                                                               
			
            gpioPWM(13, 3*power);
            gpioPWM(19, 5*power);

			return string("links");


    }else if(inputStr.compare("d")==0){                           // rechts Kurve 
		if(lastCommand == 'd'){
			if(power < 10){
				power++;
			}
			}else{
				power = 1;
				lastCommand = 'd';
			}
			cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;
			gpioWrite(6, 1);                    
            gpioWrite(26, 0);                                                                 
			r
            gpioPWM(13, 5*power);
            gpioPWM(19, 3*power);

			return string("links");



    }else if((inputStr.compare("k")==0){                           // nach oben
		if(lastCommand == 'k'){
			if(power < 10){
				power++;
			}
			}else{
				power = 1;
				lastCommand = 'k';
			}
			cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;
			gpioWrite(5, 1);                    

			
            gpioPWM(5, 5*power);
            

			return string("hoch");

    }else if((inputStr.compare("l")==0){                           // nach oben
		if(lastCommand == 'l'){
			if(power < 10){
				power++;
			}
			}else{
				power = 1;
				lastCommand = 'l';
			}
			cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;
			gpioWrite(5, 0);                    

			
            gpioPWM(5, 5*power);
            

			return string("low");

    

	}else{
		return string("unknown command");
		//gpioTerminate();
	}



	return response;return string("vorwärts");


}













/*
string MyServer::myResponse(string inputStr){
	string response;

	if(inputStr.compare("w")==0){
		if(lastCommand == 'w'){
			if(power < 10){
				power++;
			}
		}else{
			power = 1;
			lastCommand = 'w';
		}
		cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;

		gpioWrite(6, 1);
		gpioPWM(13, 5*power);

		return string("vorwärts");
	}else if(inputStr.compare("s")==0){
		if(lastCommand == 's'){
			if(power < 10){
				power++;
			}
			}else{
				power = 1;
				lastCommand = 's';
			}
			cout<<"last: "<<lastCommand<<"\t power: "<<power<<endl;
			gpioWrite(6, 0);
			gpioPWM(13, 5*power);
				return string("bremsen");
	}else{
		return string("unknown command");
		//gpioTerminate();
	}
	return response;return string("vorwärts");


}

*/







/*
 * #include <unistd.h>
#include <pigpio.h>
#include <iostream>
using namespace std;
int main(){

    if (gpioInitialise()<0){
        return 1;
    }
    int x;
    gpioSetMode(6, PI_OUTPUT);
    gpioSetMode(5, PI_OUTPUT);
    gpioSetMode(26, PI_OUTPUT);
    gpioWrite(6, 1);
    gpioWrite(5, 1);
    gpioWrite(26, 1);
    gpioSetMode(13,PI_ALT0);
    gpioSetMode(19,PI_ALT5);
    gpioSetPWMrange(13, 255);
    gpioSetPWMrange(19, 255);

    /*gpioPWM(13, 25);
    gpioPWM(19, 25);
    sleep(4);
    gpioPWM(19, 0);
    gpioPWM(13, 50);
    gpioPWM(19, 50);
    sleep(4);
    gpioPWM(13, 75);
    gpioPWM(19, 75);
    sleep(4);

    cin >>x;
    while(x!=3){
        if(x==1){
        gpioPWM(13, 25);
        sleep(4);
        gpioPWM(13, 50);
        sleep(4);
        gpioPWM(13, 75);
        sleep(4);
        }
        if(x==0){
            gpioPWM(19, 25);
            sleep(4);
            gpioPWM(19, 50);
            sleep(4);
            gpioPWM(19, 75);
            sleep(4);
        }
        if(x==2){
            gpioPWM(13, 10);
            gpioPWM(19, 25);
            sleep(5);
            gpioPWM(13, 25);
            gpioPWM(19, 10);
            sleep(5);
        }
        gpioPWM(19, 0);
        gpioPWM(13, 0);
        cout<<"in\n";
        cin >>x;

    }

    gpioTerminate();


}
*/

