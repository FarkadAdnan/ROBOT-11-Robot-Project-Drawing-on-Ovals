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
#if 0
void dump(uint8_t* p, int l){
    int i;
    for (i = 0; i < l; i++){
	SerialUSB.print(p[i], HEX);
	SerialUSB.print(" ");
    }
    SerialUSB.println("");
}
#endif
void AccelStepper::moveTo(long absolute){
    if (_targetPos != absolute)
    {
	_targetPos = absolute;
	computeNewSpeed();
    }
}
void AccelStepper::move(long relative){
    moveTo(_currentPos + relative);
}
boolean AccelStepper::runSpeed(){
    if (!_stepInterval)
	return false;
    unsigned long time = micros();
    unsigned long nextStepTime = _lastStepTime + _stepInterval;
    if (   ((nextStepTime >= _lastStepTime) && ((time >= nextStepTime) || (time < _lastStepTime)))
	|| ((nextStepTime < _lastStepTime) && ((time >= nextStepTime) && (time < _lastStepTime))))
    {
	if (_direction == DIRECTION_CW){
	    _currentPos += 1;
	}
	else{
	    _currentPos -= 1;
	}
	step(_currentPos);

	_lastStepTime = time;
	return true;
    }
    else{
	return false;
    }
}

long AccelStepper::distanceToGo(){
    return _targetPos - _currentPos;
}

long AccelStepper::targetPosition(){
    return _targetPos;
}

long AccelStepper::currentPosition(){
    return _currentPos;
}
void AccelStepper::setCurrentPosition(long position){
    _targetPos = _currentPos = position;
    _n = 0;
    _stepInterval = 0;
}

void AccelStepper::computeNewSpeed(){
    long distanceTo = distanceToGo();
    long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)); 
    if (distanceTo == 0 && stepsToStop <= 1){
	_stepInterval = 0;
	_speed = 0.0;
	_n = 0;
	return;
    }

    if (distanceTo > 0){
	if (_n > 0){
	    if ((stepsToStop >= distanceTo) || _direction == DIRECTION_CCW)
		_n = -stepsToStop; // Start deceleration
	}
	else if (_n < 0){
	    if ((stepsToStop < distanceTo) && _direction == DIRECTION_CW)
		_n = -_n; // Start accceleration
	}
    }
    else if (distanceTo < 0){
	if (_n > 0){
	    if ((stepsToStop >= -distanceTo) || _direction == DIRECTION_CW)
		_n = -stepsToStop; // Start deceleration
	}
	else if (_n < 0){
	    if ((stepsToStop < -distanceTo) && _direction == DIRECTION_CCW)
		_n = -_n; // Start accceleration
	}
    }
    if (_n == 0){
	_cn = _c0;
	_direction = (distanceTo > 0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    else{
	_cn = _cn - ((2.0 * _cn) / ((4.0 * _n) + 1)); // Equation 13
	_cn = max(_cn, _cmin); 
    }
    _n++;
    _stepInterval = _cn;
    _speed = 1000000.0 / _cn;
    if (_direction == DIRECTION_CCW)
	_speed = -_speed;

#if 0
    SerialUSB.println(_speed);
    SerialUSB.println(_acceleration);
    SerialUSB.println(_cn);
    SerialUSB.println(_c0);
    SerialUSB.println(_n);
    SerialUSB.println(_stepInterval);
    SerialUSB.println(distanceTo);
    SerialUSB.println(stepsToStop);
    SerialUSB.println("-----");
#endif
}
boolean AccelStepper::run(){
    if (runSpeed())
	computeNewSpeed();
    return _speed != 0.0 || distanceToGo() != 0;
}

AccelStepper::AccelStepper(uint8_t interface, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, bool enable){
    _interface = interface;
    _currentPos = 0;
    _targetPos = 0;
    _speed = 0.0;
    _maxSpeed = 1.0;
    _acceleration = 0.0;
    _sqrt_twoa = 1.0;
    _stepInterval = 0;
    _minPulseWidth = 1;
    _enablePin = 0xff;
    _lastStepTime = 0;
    _pin[0] = pin1;
    _pin[1] = pin2;
    _pin[2] = pin3;
    _pin[3] = pin4;

    _n = 0;
    _c0 = 0.0;
    _cn = 0.0;
    _cmin = 1.0;
    _direction = DIRECTION_CCW;

    int i;
    for (i = 0; i < 4; i++)
	_pinInverted[i] = 0;
    if (enable)
	enableOutputs();
    // Some reasonable default
    setAcceleration(1);
}

AccelStepper::AccelStepper(void (*forward)(), void (*backward)()){
    _interface = 0;
    _currentPos = 0;
    _targetPos = 0;
    _speed = 0.0;
    _maxSpeed = 1.0;
    _acceleration = 0.0;
    _sqrt_twoa = 1.0;
    _stepInterval = 0;
    _minPulseWidth = 1;
    _enablePin = 0xff;
    _lastStepTime = 0;
    _pin[0] = 0;
    _pin[1] = 0;
    _pin[2] = 0;
    _pin[3] = 0;
    _forward = forward;
    _backward = backward;

    _n = 0;
    _c0 = 0.0;
    _cn = 0.0;
    _cmin = 1.0;
    _direction = DIRECTION_CCW;

    int i;
    for (i = 0; i < 4; i++)
	_pinInverted[i] = 0;
    setAcceleration(1);
}
void AccelStepper::setMaxSpeed(float speed){
    if (_maxSpeed != speed)
    {
	_maxSpeed = speed;
	_cmin = 1000000.0 / speed;
	if (_n > 0)
	{
	    _n = (long)((_speed * _speed) / (2.0 * _acceleration)); // Equation 16
	    computeNewSpeed();
	}
 }
}

void AccelStepper::setAcceleration(float acceleration){
    if (acceleration == 0.0)
	return;
    if (_acceleration != acceleration){
	_n = _n * (_acceleration / acceleration);
	_c0 = 0.676 * sqrt(2.0 / acceleration) * 1000000.0; 
	_acceleration = acceleration;
	computeNewSpeed();
    }
}

void AccelStepper::setSpeed(float speed){
    if (speed == _speed)
        return;
    speed = constrain(speed, -_maxSpeed, _maxSpeed);
    if (speed == 0.0)
	_stepInterval = 0;
    else{
	_stepInterval = fabs(1000000.0 / speed);
	_direction = (speed > 0.0) ? DIRECTION_CW : DIRECTION_CCW;
    }
    _speed = speed;
}

float AccelStepper::speed(){
    return _speed;
}

void AccelStepper::step(long step){
    switch (_interface){
        case FUNCTION:
            step0(step);
            break;

	case DRIVER:
	    step1(step);
	    break;
    
	case FULL2WIRE:
	    step2(step);
	    break;
    
	case FULL3WIRE:
	    step3(step);
	    break;  

	case FULL4WIRE:
	    step4(step);
	    break;  

	case HALF3WIRE:
	    step6(step);
	    break;  
		
	case HALF4WIRE:
	    step8(step);
	    break;  
    }
}

void AccelStepper::setOutputPins(uint8_t mask){
    uint8_t numpins = 2;
    if (_interface == FULL4WIRE || _interface == HALF4WIRE)
	numpins = 4;
    else if (_interface == FULL3WIRE || _interface == HALF3WIRE)
	numpins = 3;
    uint8_t i;
    for (i = 0; i < numpins; i++)
	digitalWrite(_pin[i], (mask & (1 << i)) ? (HIGH ^ _pinInverted[i]) : (LOW ^ _pinInverted[i]));
}

void AccelStepper::step0(long step){
  if (_speed > 0)
    _forward();
  else
    _backward();
}

void AccelStepper::step1(long step){
    setOutputPins(_direction ? 0b10 : 0b00);
    setOutputPins(_direction ? 0b11 : 0b01);
    delayMicroseconds(_minPulseWidth);
    setOutputPins(_direction ? 0b10 : 0b00); // step LOW

}

void AccelStepper::step2(long step){
    switch (step & 0x3){
	case 0: /* 01 */
	    setOutputPins(0b10);
	    break;

	case 1: /* 11 */
	    setOutputPins(0b11);
	    break;

	case 2: /* 10 */
	    setOutputPins(0b01);
	    break;

	case 3: /* 00 */
	    setOutputPins(0b00);
	    break;
    }
}
void AccelStepper::step3(long step){
    switch (step % 3){
	case 0:    // 100
	    setOutputPins(0b100);
	    break;

	case 1:    // 001
	    setOutputPins(0b001);
	    break;

	case 2:    //010
	    setOutputPins(0b010);
	    break;
	    
    }
}

void AccelStepper::step4(long step){
    switch (step & 0x3)
    {
	case 0:    // 1010
	    setOutputPins(0b0101);
	    break;

	case 1:    // 0110
	    setOutputPins(0b0110);
	    break;

	case 2:    //0101
	    setOutputPins(0b1010);
	    break;

	case 3:    //1001
	    setOutputPins(0b1001);
	    break;
    }
}

void AccelStepper::step6(long step){
    switch (step % 6){
	case 0:    // 100
	    setOutputPins(0b100);
            break;
	    
        case 1:    // 101
	    setOutputPins(0b101);
            break;
	    
	case 2:    // 001
	    setOutputPins(0b001);
            break;
	    
        case 3:    // 011
	    setOutputPins(0b011);
            break;
	    
	case 4:    // 010
	    setOutputPins(0b010);
            break;
	    
	case 5:    // 011
	    setOutputPins(0b110);
            break;
	    
    }
}

void AccelStepper::step8(long step){
    switch (step & 0x7){
	case 0:    // 1000
	    setOutputPins(0b0001);
            break;
	    
        case 1:    // 1010
	    setOutputPins(0b0101);
            break;
	    
	case 2:    // 0010
	    setOutputPins(0b0100);
            break;
	    
        case 3:    // 0110
	    setOutputPins(0b0110);
            break;
	    
	case 4:    // 0100
	    setOutputPins(0b0010);
            break;
	    
        case 5:    //0101
	    setOutputPins(0b1010);
            break;
	    
	case 6:    // 0001
	    setOutputPins(0b1000);
            break;
	    
        case 7:    //1001
	    setOutputPins(0b1001);
            break;
    }
}
    
void    AccelStepper::disableOutputs(){   
    if (! _interface) return;
    setOutputPins(0); // Handles inversion automatically
    if (_enablePin != 0xff)
        digitalWrite(_enablePin, LOW ^ _enableInverted);
}
void    AccelStepper::enableOutputs(){
    if (! _interface) 
	return;
    pinMode(_pin[0], OUTPUT);
    pinMode(_pin[1], OUTPUT);
    if (_interface == FULL4WIRE || _interface == HALF4WIRE){
        pinMode(_pin[2], OUTPUT);
        pinMode(_pin[3], OUTPUT);
    }
    else if (_interface == FULL3WIRE || _interface == HALF3WIRE){
        pinMode(_pin[2], OUTPUT);
    }

    if (_enablePin != 0xff){
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, HIGH ^ _enableInverted);
    }
}

void AccelStepper::setMinPulseWidth(unsigned int minWidth){
    _minPulseWidth = minWidth;
}

void AccelStepper::setEnablePin(uint8_t enablePin){
    _enablePin = enablePin;

    if (_enablePin != 0xff){
        pinMode(_enablePin, OUTPUT);
        digitalWrite(_enablePin, HIGH ^ _enableInverted);
    }
}

void AccelStepper::setPinsInverted(bool directionInvert, bool stepInvert, bool enableInvert){
    _pinInverted[0] = stepInvert;
    _pinInverted[1] = directionInvert;
    _enableInverted = enableInvert;
}

void AccelStepper::setPinsInverted(bool pin1Invert, bool pin2Invert, bool pin3Invert, bool pin4Invert, bool enableInvert){    
    _pinInverted[0] = pin1Invert;
    _pinInverted[1] = pin2Invert;
    _pinInverted[2] = pin3Invert;
    _pinInverted[3] = pin4Invert;
    _enableInverted = enableInvert;
}

void AccelStepper::runToPosition(){
    while (run())
	;
}

boolean AccelStepper::runSpeedToPosition(){
    if (_targetPos == _currentPos)
	return false;
    if (_targetPos >_currentPos)
	_direction = DIRECTION_CW;
    else
	_direction = DIRECTION_CCW;
    return runSpeed();
}

void AccelStepper::runToNewPosition(long position){
    moveTo(position);
    runToPosition();
}

void AccelStepper::stop(){
    if (_speed != 0.0)
    {    
	long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)) + 1; // Equation 16 (+integer rounding)
	if (_speed > 0)
	    move(stepsToStop);
	else
	    move(-stepsToStop);
    }
}
