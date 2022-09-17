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
  
#ifndef AccelStepper_h
#define AccelStepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

 #undef round
 
class AccelStepper
{
public:
    
    typedef enum
    {
	FUNCTION  = 0, ///< Use the functional interface, implementing your own driver functions (internal use only)
	DRIVER    = 1, ///< Stepper Driver, 2 driver pins required
	FULL2WIRE = 2, ///< 2 wire stepper, 2 motor pins required
	FULL3WIRE = 3, ///< 3 wire stepper, such as HDD spindle, 3 motor pins required
        FULL4WIRE = 4, ///< 4 wire full stepper, 4 motor pins required
	HALF3WIRE = 6, ///< 3 wire half stepper, such as HDD spindle, 3 motor pins required
	HALF4WIRE = 8  ///< 4 wire half stepper, 4 motor pins required
    } MotorInterfaceType;
 
    AccelStepper(uint8_t interface = AccelStepper::FULL4WIRE, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true);

   
    AccelStepper(void (*forward)(), void (*backward)());
    
       void    moveTo(long absolute); 

      void    move(long relative);

        boolean run();

        boolean runSpeed();

       void    setMaxSpeed(float speed);

       void    setAcceleration(float acceleration);

        void    setSpeed(float speed);

     float   speed();

     long    distanceToGo();

     long    targetPosition();

       long    currentPosition();  

       void    setCurrentPosition(long position);  
    
      void    runToPosition();

       boolean runSpeedToPosition();

      void    runToNewPosition(long position);

      void stop();

   
    virtual void    disableOutputs();

     virtual void    enableOutputs();

     void    setMinPulseWidth(unsigned int minWidth);

       void    setEnablePin(uint8_t enablePin = 0xff);

       void    setPinsInverted(bool directionInvert = false, bool stepInvert = false, bool enableInvert = false);

        void    setPinsInverted(bool pin1Invert, bool pin2Invert, bool pin3Invert, bool pin4Invert, bool enableInvert);

protected:

      typedef enum
    {
	DIRECTION_CCW = 0,  ///< Clockwise
        DIRECTION_CW  = 1   ///< Counter-Clockwise
    } Direction;

     void           computeNewSpeed();

      virtual void   setOutputPins(uint8_t mask);

       virtual void   step(long step);

    /// Called to execute a step using stepper functions (pins = 0) Only called when a new step is
    /// required. Calls _forward() or _backward() to perform the step
    /// \param[in] step The current step phase number (0 to 7)
    virtual void   step0(long step);

    /// Called to execute a step on a stepper driver (ie where pins == 1). Only called when a new step is
    /// required. Subclasses may override to implement new stepping
    /// interfaces. The default sets or clears the outputs of Step pin1 to step, 
    /// and sets the output of _pin2 to the desired direction. The Step pin (_pin1) is pulsed for 1 microsecond
    /// which is the minimum STEP pulse width for the 3967 driver.
    /// \param[in] step The current step phase number (0 to 7)
    virtual void   step1(long step);

    /// Called to execute a step on a 2 pin motor. Only called when a new step is
    /// required. Subclasses may override to implement new stepping
    /// interfaces. The default sets or clears the outputs of pin1 and pin2
    /// \param[in] step The current step phase number (0 to 7)
    virtual void   step2(long step);

    /// Called to execute a step on a 3 pin motor, such as HDD spindle. Only called when a new step is
    /// required. Subclasses may override to implement new stepping
    /// interfaces. The default sets or clears the outputs of pin1, pin2,
    /// pin3
    /// \param[in] step The current step phase number (0 to 7)
    virtual void   step3(long step);

    /// Called to execute a step on a 4 pin motor. Only called when a new step is
    /// required. Subclasses may override to implement new stepping
    /// interfaces. The default sets or clears the outputs of pin1, pin2,
    /// pin3, pin4.
    /// \param[in] step The current step phase number (0 to 7)
    virtual void   step4(long step);

    /// Called to execute a step on a 3 pin motor, such as HDD spindle. Only called when a new step is
    /// required. Subclasses may override to implement new stepping
    /// interfaces. The default sets or clears the outputs of pin1, pin2,
    /// pin3
    /// \param[in] step The current step phase number (0 to 7)
    virtual void   step6(long step);

    /// Called to execute a step on a 4 pin half-steper motor. Only called when a new step is
    /// required. Subclasses may override to implement new stepping
    /// interfaces. The default sets or clears the outputs of pin1, pin2,
    /// pin3, pin4.
    /// \param[in] step The current step phase number (0 to 7)
    virtual void   step8(long step);

private:
    /// Number of pins on the stepper motor. Permits 2 or 4. 2 pins is a
    /// bipolar, and 4 pins is a unipolar.
    uint8_t        _interface;          // 0, 1, 2, 4, 8, See MotorInterfaceType

    /// Arduino pin number assignments for the 2 or 4 pins required to interface to the
    /// stepper motor or driver
    uint8_t        _pin[4];

    /// Whether the _pins is inverted or not
    uint8_t        _pinInverted[4];

    /// The current absolution position in steps.
    long           _currentPos;    // Steps

    /// The target position in steps. The AccelStepper library will move the
    /// motor from the _currentPos to the _targetPos, taking into account the
    /// max speed, acceleration and deceleration
    long           _targetPos;     // Steps

    /// The current motos speed in steps per second
    /// Positive is clockwise
    float          _speed;         // Steps per second

    /// The maximum permitted speed in steps per second. Must be > 0.
    float          _maxSpeed;

    /// The acceleration to use to accelerate or decelerate the motor in steps
    /// per second per second. Must be > 0
    float          _acceleration;
    float          _sqrt_twoa; // Precomputed sqrt(2*_acceleration)

    /// The current interval between steps in microseconds.
    /// 0 means the motor is currently stopped with _speed == 0
    unsigned long  _stepInterval;

    /// The last step time in microseconds
    unsigned long  _lastStepTime;

    /// The minimum allowed pulse width in microseconds
    unsigned int   _minPulseWidth;

    /// Is the direction pin inverted?
    ///bool           _dirInverted; /// Moved to _pinInverted[1]

    /// Is the step pin inverted?
    ///bool           _stepInverted; /// Moved to _pinInverted[0]

    /// Is the enable pin inverted?
    bool           _enableInverted;

    /// Enable pin for stepper driver, or 0xFF if unused.
    uint8_t        _enablePin;

    /// The pointer to a forward-step procedure
    void (*_forward)();

    /// The pointer to a backward-step procedure
    void (*_backward)();

    /// The step counter for speed calculations
    long _n;

    /// Initial step size in microseconds
    float _c0;

    /// Last step size in microseconds
    float _cn;

    /// Min step size in microseconds based on maxSpeed
    float _cmin; // at max speed

    /// Current direction motor is spinning in
    boolean _direction; // 1 == CW

};

/// @example Random.pde
/// Make a single stepper perform random changes in speed, position and acceleration

/// @example Overshoot.pde
///  Check overshoot handling
/// which sets a new target position and then waits until the stepper has 
/// achieved it. This is used for testing the handling of overshoots

/// @example MultiStepper.pde
/// Shows how to multiple simultaneous steppers
/// Runs one stepper forwards and backwards, accelerating and decelerating
/// at the limits. Runs other steppers at the same time

/// @example ConstantSpeed.pde
/// Shows how to run AccelStepper in the simplest,
/// fixed speed mode with no accelerations

/// @example Blocking.pde 
/// Shows how to use the blocking call runToNewPosition
/// Which sets a new target position and then waits until the stepper has 
/// achieved it.

/// @example AFMotor_MultiStepper.pde
/// Control both Stepper motors at the same time with different speeds
/// and accelerations. 

/// @example AFMotor_ConstantSpeed.pde
/// Shows how to run AccelStepper in the simplest,
/// fixed speed mode with no accelerations

/// @example ProportionalControl.pde
/// Make a single stepper follow the analog value read from a pot or whatever
/// The stepper will move at a constant speed to each newly set posiiton, 
/// depending on the value of the pot.

/// @example Bounce.pde
/// Make a single stepper bounce from one limit to another, observing
/// accelrations at each end of travel

/// @example Quickstop.pde
/// Check stop handling.
/// Calls stop() while the stepper is travelling at full speed, causing
/// the stepper to stop as quickly as possible, within the constraints of the
/// current acceleration.

/// @example MotorShield.pde
/// Shows how to use AccelStepper to control a 3-phase motor, such as a HDD spindle motor
/// using the Adafruit Motor Shield http://www.ladyada.net/make/mshield/index.html.

/// @example DualMotorShield.pde
/// Shows how to use AccelStepper to control 2 x 2 phase steppers using the 
/// Itead Studio Arduino Dual Stepper Motor Driver Shield
/// model IM120417015

#endif 
