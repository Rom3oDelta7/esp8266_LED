/*

ESP8266 library for managing LEDs

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
  and we use a pointer to the object to allow this function to be used for all LED objects
 */
void _LEDToggle (LED* led) {
   led->toggleState();
}

/*
 This callback handles alternating between 2 RGB colors
 */
void _LEDAlternate (LED* led) {
   led->alternateRGB();
}

/*
 LED constructors
*/

// single pin LED
LED::LED (const uint8_t pin) {
   _LEDpin = pin;
   pinMode(_LEDpin, OUTPUT);                 // note: type default is SINGLE
}

// RGB LED
LED::LED (const uint8_t redPin, const uint8_t greenPin, const uint8_t bluePin, LEDType ledType) {
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


/*
 Set the color for an RGB LED
 Silently ignore if this is an single-color LED
 */
void LED::setColor (const LEDColor color) {
   if ( _type != LEDType::SINGLE ) {
      _color = color;
      LOG(PSTR("RGB color set to %0x\n"), (int)color);
    }
}

/*
 Return the currently set color of an RGB LED
*/
LEDColor LED::getColor (void) {
   return _color;                                  // this will be WHITE if this is not an RGB LED
}

/*
 Set RGB LED alternating colors
*/
void LED::setAlternatingColors(const LEDColor color1, const LEDColor color2) {
   _altColor1 = color1;
   _altColor2 = color2;
   _color = _altColor1;
}


/*
 Illuminate an LED
*/

// Single-color LED
void LED::_illuminate (const bool mode) {
   if ( mode ) {
      INFO(F("Solid LED"), F("ON"));
      digitalWrite(_LEDpin, HIGH);
   } else {
      INFO(F("Solid LED"), F("OFF"));
      digitalWrite(_LEDpin, LOW);
   }
   
}

// RGB LED
void LED::_illuminate (const LEDColor targetColor) {
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
   analogWrite(_redPin, redPWM);
   analogWrite(_greenPin, greenPWM);
   analogWrite(_bluePin, bluePWM); 
}


/*
 This ISR is called by the physical pin change "C" function to change the current (temporal) state for blinking
*/
void LED::toggleState (void) {
   os_intr_lock();                           // disable interrupts
   switch ( _type ) {
   case LEDType::SINGLE:
      _illuminated = !_illuminated;
      _illuminate(_illuminated);
      break;

   case LEDType::CATHODE:
   case LEDType::ANODE:
      if ( _illuminated ) {
         _illuminated = false;
         _illuminate(LEDColor::NONE);       // this results in an analogWrite(0), which turns off the LED
      } else {
         _illuminated = true;
         _illuminate(_color);
      }
      break;

   default:
      break;

   }
   os_intr_unlock();                        // enable interrupts again
}

/*
 ISR: Switch between 2 RGB colors - called by timer callout
*/
void LED::alternateRGB(void) {
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
void LED::setState(const LEDState ledState, const uint32_t interval) {
   if ( _timerArmed ) {
      // reset the timer whenever we change the state
      os_timer_disarm(&_timer);
      _timerArmed = false;
   }
   INFO(F("State set to"), (int)ledState);
   INFO(F("Interval"), interval);
   switch ( ledState ) {
   case LEDState::ON:
   case LEDState::BLINK_ON:
      _illuminated = true;
      if ( _type == LEDType::SINGLE ) {
         _illuminate(true);
      } else {
         _illuminate(_color);
      }
      if ( ledState == LEDState::BLINK_ON ) {
         // blink, with initial state ON - min interval is 5 - see ESP SDK documentation
         os_timer_setfn(&_timer, reinterpret_cast<ETSTimerFunc*>(&_LEDToggle), reinterpret_cast<void*>(this));
         os_timer_arm(&_timer, interval >= 5 ? interval : 5, true);
         _timerArmed = true;
      }
      break;

   case LEDState::OFF:
   case LEDState::BLINK_OFF:
      _illuminated = false;
      if ( _type == LEDType::SINGLE ) {
         _illuminate(false);
      } else {
         _illuminate(LEDColor::NONE);
      }
      if ( ledState == LEDState::BLINK_OFF ) {
         // blinking, but initial state is OFF
         os_timer_setfn(&_timer, reinterpret_cast<ETSTimerFunc*>(&_LEDToggle), reinterpret_cast<void*>(this));
         os_timer_arm(&_timer, interval >= 5 ? interval : 5, true);
         _timerArmed = true;
      }
      break;

   case LEDState::ALTERNATE:
      // alternate between 2 RGB colors
      os_timer_setfn(&_timer, reinterpret_cast<ETSTimerFunc*>(&_LEDAlternate), reinterpret_cast<void*>(this));
      os_timer_arm(&_timer, interval >= 5 ? interval : 5, true);
      _timerArmed = true;
      break;

   default:
      break;
   }
}
