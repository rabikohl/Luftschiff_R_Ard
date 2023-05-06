#include <iostream>
#include <pigpio.h>

using namespace std;

int main(){

	if(gpioInitialise()<0){
		cout<<"Initialisierung fehlgeschlagen!"<<endl;
		return 1;
	}
	int motor=-1,richtung=-1,geschwindigkeit=-1;
	gpioSetMode(5,PI_OUTPUT);
	gpioSetMode(6,PI_OUTPUT);
	gpioSetMode(15,PI_OUTPUT);
	gpioSetMode(26,PI_OUTPUT);
	gpioSetMode(13,PI_ALT0);
	gpioSetMode(18,PI_ALT0);
	gpioSetMode(19,PI_ALT5);
	gpioSetPWMrange(13,255);
	gpioSetPWMrange(18,255);
	gpioSetPWMrange(19,255);
	gpioWrite(5,1);
	gpioPWM(13,0);
	gpioPWM(18,0);
	gpioPWM(19,0);

	while(true){

		do{
			cout<<"Motor: ";
			cin >>motor;
			if(motor < 0 || motor > 3){
				cout<<"Falscher Input!";
			}else{
				break;
			}
		}while(true);
		do{
			cout<<"Richtung: ";
			cin>>richtung;
			if(richtung < 0 || richtung > 1){
				cout<<"Falscher Input!";
			}else{
				break;
			}
		}while(true);

		do{
			cout<<"Geschwindigkeit: ";
			cin>>geschwindigkeit;
			if(geschwindigkeit < 0 || geschwindigkeit > 255){
				cout<<"Falscher Input!";
			}else{
				break;
			}
		}while(true);

		if(motor==1){
			gpioWrite(6,richtung);
			gpioPWM(13,geschwindigkeit);
		}
		if(motor==2){
			gpioWrite(26,richtung);
			gpioPWM(19,geschwindigkeit);
		}
		if(motor==3){
			gpioWrite(15,richtung);
			gpioPWM(18,geschwindigkeit);
		}
		if(motor==0){
			break;
		}
	}
	gpioTerminate();
    return 0;
}
