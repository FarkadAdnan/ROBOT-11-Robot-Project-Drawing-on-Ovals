/*
 By:Farkad Adnan
 https://linktr.ee/farkadadnan
 E-mail: farkad.hpfa95@gmail.com
 inst : farkadadnan
 #farkadadnan , #farkad_adnan , فرقد عدنان#
 FaceBook: كتاب عالم الاردوينو
 inst : arduinobook
 #كتاب_عالم_الاردوينو  #كتاب_عالم_الآردوينو 
 */
 
 #include "AccelStepper.h"
#include <Servo.h>
#include "SerialCommand.h"
//#include <avr/eeprom.h>
#include "button.h"

#define initSting "EBBv13_and_above Protocol emulated by Eggduino-Firmware V1.6a"
#define step1 5
#define dir1 6
#define enableRotMotor 11
#define rotMicrostep 16  //MicrostepMode (default 16)
#define step2 7
#define dir2 8
#define enablePenMotor 11
#define penMicrostep 16 

#define servoPin 4 
AccelStepper rotMotor(1, step1, dir1);
AccelStepper penMotor(1, step2, dir2);
Servo penServo;
SerialCommand SCmd;
#ifdef prgButton
	Button prgButtonToggle(prgButton, setprgButtonState);
#endif
#ifdef penToggleButton
	Button penToggle(penToggleButton, doTogglePen);
#endif
#ifdef motorsButton
	Button motorsToggle(motorsButton, toggleMotors);
#endif
int penMin=0;
int penMax=0;
int penUpPos=52;  
int penDownPos=60; 
int servoRateUp=0; 
int servoRateDown=0; 
long rotStepError=0;
long penStepError=0;
int penState=penUpPos;
uint32_t nodeCount=0;
unsigned int layer=0;
boolean prgButtonState=0;
uint8_t rotStepCorrection = 16/rotMicrostep ;
uint8_t penStepCorrection = 16/penMicrostep ; 
float rotSpeed=0;
float penSpeed=0; 
boolean motorsEnabled = 0;

void setup() {   
	SerialUSB.begin(9600);
	makeComInterface();
	initHardware();
  SerialUSB.print ("hi");
}

void loop() {
	moveOneStep();

	SCmd.readSerial();
  
#ifdef penToggleButton
	penToggle.check();
#endif

#ifdef motorsButton
	motorsToggle.check();
#endif

#ifdef prgButton
	prgButtonToggle.check();
#endif
}
