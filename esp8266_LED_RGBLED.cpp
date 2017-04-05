/*

ESP8266 library for managing LEDs
Functions for RGB LEDs

Copyright 2017 Rob Redford
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/.
*/

#include <esp8266_LED.h>

//#define DEBUG_INFO
//#define DEBUG_LOG

#include "DebugLib.h"

/*
This callback funciton is called by the ESP OS timer interrupt to toggle the LED's state for blinking
Note that as you cannot get the address of a class member function, this has to exist outside the class
and we use a pointer to the object to allow this function to access the LED object
*/
void _RGBToggle (RGBLED* led) {
   led->_toggleState();
}


/*
This callback handles alternating between 2 RGB colors
*/
void _RGBAlternate (RGBLED* led) {
   led->_alternateRGB();
}


// RGB LED Constructor
RGBLED::RGBLED (const uint8_t redPin, const uint8_t greenPin, const uint8_t bluePin, LEDType ledType) {
   INFO(F("constructing LED"), F("RGB"));
   _redPin = redPin;
   _greenPin = greenPin;
   _bluePin = bluePin;
   pinMode(_redPin, OUTPUT);
   pinMode(_greenPin, OUTPUT);
   pinMode(_bluePin, OUTPUT);
   if ( ledType != LEDType::SINGLE ) {                    // this mode is disallowed
      _type = ledType;
   } else {
      _type = LEDType::CATHODE;
   }
}


// illuminate an RGB LED
void RGBLED::_illuminate (const LEDColor targetColor) {
   int     color = static_cast<int>(targetColor);
   uint8_t redPWM = (color & 0xFF0000) >> 16;
   uint8_t greenPWM = (color & 0xFF00) >> 8;
   uint8_t bluePWM = (color & 0xFF);
   LOG(PSTR("INFO> Illumination: R(%0x), G(%0x), B(%0x)\n"), redPWM, greenPWM, bluePWM);
   if ( _type == LEDType::ANODE ) {
      // for common anode configs, the logic is active LOW (turn on LED with 0), so we invert the values
      redPWM = map(redPWM, 0, 0xFF, 0xFF, 0);
      greenPWM = map(greenPWM, 0, 0xFF, 0xFF, 0);
      bluePWM = map(bluePWM, 0, 0xFF, 0xFF, 0);
   }

   // if the color is solid, then use digitalWrite to avoid any PWM-related issues
   if ( redPWM == 0xFF ) {
      digitalWrite(_redPin, HIGH);
   } else if ( redPWM == 0 ) {
      digitalWrite(_redPin, LOW);
   } else {
      analogWrite(_redPin, redPWM);
   }

   if ( greenPWM == 0xFF ) {
      digitalWrite(_greenPin, HIGH);
   } else if ( greenPWM == 0 ) {
      digitalWrite(_greenPin, LOW);
   } else {
      analogWrite(_greenPin, greenPWM);
   }

   if ( bluePWM == 0xFF ) {
      digitalWrite(_bluePin, HIGH);
   } else if ( bluePWM == 0 ) {
      digitalWrite(_bluePin, LOW);
   } else {
      analogWrite(_bluePin, bluePWM);
   }
}


/*
This ISR is called by the physical pin change "C" function to change the current (temporal) state for blinking
*/
void RGBLED::_toggleState (void) {
   os_intr_lock();                           // disable interrupts
   if ( _illuminated ) {
      _illuminated = false;
      _illuminate(LEDColor::NONE);
   } else {
      _illuminated = true;
      _illuminate(_color);
   }
   os_intr_unlock();                        // enable interrupts again
}

/*
ISR: Switch between 2 RGB colors - called by timer callout
*/
void RGBLED::_alternateRGB(void) {
   os_intr_lock();
   if ( _color == _altColor1 ) {
      _color = _altColor2;
   } else {
      _color = _altColor1;
   }
   _illuminated = true;
   _illuminate(_color);
   os_intr_unlock();
}

/*
Set a new target state of the LED
For blinking, the state indicates the initial condition of the LED - this allows us to alternate between 2 LEDs
*/
void RGBLED::setState(const LEDState ledState, const uint32_t interval) {
   if ( _timerArmed ) {
      // reset the timer whenever we change the state
      os_timer_disarm(&_timer);
      _timerArmed = false;
   }
   INFO(F("State set to"), (int)ledState);
   INFO(F("Interval"), interval);
   _state = ledState;
   switch ( ledState ) {
   case LEDState::ON:
   case LEDState::BLINK_ON:
      _illuminated = true;
      _illuminate(_color);

      if ( ledState == LEDState::BLINK_ON ) {
         // blink, with initial state ON - min interval is 5 - see ESP SDK documentation
         os_timer_setfn(&_timer, reinterpret_cast<ETSTimerFunc*>(&_RGBToggle), reinterpret_cast<void*>(this));
         os_timer_arm(&_timer, interval >= 5 ? interval : 5, true);
         _timerArmed = true;
      }
      break;

   case LEDState::OFF:
   case LEDState::BLINK_OFF:
      _illuminated = false;
       _illuminate(LEDColor::NONE);

      if ( ledState == LEDState::BLINK_OFF ) {
         // blinking, but initial state is OFF
         os_timer_setfn(&_timer, reinterpret_cast<ETSTimerFunc*>(&_RGBToggle), reinterpret_cast<void*>(this));
         os_timer_arm(&_timer, interval >= 5 ? interval : 5, true);
         _timerArmed = true;
      }
      break;

   case LEDState::ALTERNATE:
      // alternate between 2 RGB colors
      os_timer_setfn(&_timer, reinterpret_cast<ETSTimerFunc*>(&_RGBAlternate), reinterpret_cast<void*>(this));
      os_timer_arm(&_timer, interval >= 5 ? interval : 5, true);
      _timerArmed = true;
      break;

   default:
      break;
   }
}
