/*

 ESP8266 library for managing RGB andf single-color LEDs

 Copyright 2017 Rob Redford
 This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/.
*/

#ifndef _ESP8266_LED_H_
#define _ESP8266_LED_H_

#ifndef ESP8266
#error This library is for use with an ESP8266 only
#else

#include "Arduino.h"

// ESP8266 SDK                                   Ref: https://espressif.com/sites/default/files/documentation/2c-esp8266_non_os_sdk_api_reference_en.pdf
extern "C" {
#include "user_interface.h"
}

// type for RGB LEDs
enum class LEDType:uint8_t { ANODE, CATHODE, SINGLE };

// colors for RGB LED
enum class LEDColor:uint32_t {
   RED		=	0xFF0000,
   GREEN		=  0x00FF00,
   BLUE		=	0x0000FF,
   MAGENTA	=	0xFF00FF,
   YELLOW	=	0xFFFF00,
   CYAN		=	0x00FFFF,
   WHITE		=  0xFFFFFF,
   ORANGE	=	0xFFA500,
   PURPLE	=	0x800080,
   NONE		=	0x000000,
};

// LED states
enum class LEDState:uint8_t { OFF, ON, BLINK_OFF, BLINK_ON, ALTERNATE };


class LED {
public:
   LED (const uint8_t pin);
   LED (const uint8_t redPin, const uint8_t greenPin, const uint8_t bluePin, LEDType ledType = LEDType::CATHODE);

   void       setColor(const LEDColor color);
   void       setAlternatingColors(const LEDColor color1, const LEDColor color2);
   LEDColor   getColor(void);
   void       setState(const LEDState ledState, const uint32_t interval = 500);
   void       toggleState(void);
   void       alternateRGB(void);
   
private:
   void       _illuminate(const LEDColor color);
   void       _illuminate(const bool mode);

   uint8_t    _LEDpin = 0;                          // single LED pin -or-
   uint8_t    _redPin = 0;                          // RGB LED pins
   uint8_t    _greenPin = 0;
   uint8_t    _bluePin = 0;
   volatile   LEDColor _color = LEDColor::WHITE;    // current color for RGB LEDs
   LEDColor   _altColor1 = LEDColor::RED;           // for alternating colors. Arbitrary starting values
   LEDColor   _altColor2 = LEDColor::GREEN;
   LEDType    _type = LEDType::SINGLE;              // single LED or common anode or cathode for RGB LEDs
   volatile   bool _illuminated = false;            // for blinking
   bool       _timerArmed = false;
   os_timer_t _timer;                               // ESP OS software timer (ESP8266 SDK)
};


#endif
#endif

