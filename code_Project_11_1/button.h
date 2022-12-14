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
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <Arduino.h>

#define debounceDelay 50

typedef void (*ActionCb)(void);

class Button
{
	
private:
	long debounce;
	byte state:1;
	byte lastState:1;
	byte pin;
	ActionCb action;
	Button( const Button &c );
	Button& operator=( const Button &c );
	
public:
	
	Button(byte p, ActionCb a): debounce(0), state(1), lastState(1), action(a), pin(p) {
		pinMode(pin, INPUT_PULLUP);
	}

	void check() {
		byte b = digitalRead(pin);
		long t = millis();

		if (b != lastState) {
			debounce = t;
		}

		if ((t - debounce) > debounceDelay) {
			if (b != state) {
				state = b;

				if (!state) {
					(*action)();
				}
			}
		}

		lastState = b;
	}
}; //button

#endif //__BUTTON_H__
