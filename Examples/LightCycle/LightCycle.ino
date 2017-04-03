/*
 Example sketch for esp8266_LED library
 
 Demonstration of the different modes, for both RGB and solid LEDs
 
 For common anode implementations, attach the common lead to Vcc and attach each RGB lead to the color pin through a current limiting resistor (current sink)
 For common cathode implementations, attach the common lead to ground, and attach each RGB lead to the color pin through a current limiting resistor (current source)
 
 Copyright 2017 Rob Redford
   This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/.
*/

#include <esp8266_LED.h>
#include <vector>

// RGB LED
#define RED_PIN   14			// pin assignments for ESP-12E. Change to match your confiruation
#define GREEN_PIN	12			// make sure there is a current limiting resistor attached to each color lead
#define BLUE_PIN	13

// single-color LED
#define LED_PIN    2       

std::vector<LEDColor> colors = { LEDColor::RED, LEDColor::GREEN, LEDColor::BLUE, LEDColor::MAGENTA, LEDColor::CYAN, LEDColor::WHITE, LEDColor::ORANGE, LEDColor::PURPLE };

LED		rgb(RED_PIN, GREEN_PIN, BLUE_PIN);
//LED		rgb(RED_PIN, GREEN_PIN, BLUE_PIN, LEDType::ANODE);						// constructor for common anode

LED       led(LED_PIN);


void setup ( void ) {
   Serial.begin(115200);
   while (!Serial);
}

String colorName (const LEDColor color) {
   String name;

   switch ( color ) {
   case LEDColor::RED:
      name = String("RED");
      break;
   case LEDColor::GREEN:
      name = String("GREEN");
      break;
   case LEDColor::BLUE:
      name = String("BLUE");
      break;
   case LEDColor::MAGENTA:
      name = String("MAGENTA");
      break;
   case LEDColor::YELLOW:
      name = String("YELLOW");
      break;
   case LEDColor::CYAN:
      name = String("CYAN");
      break;
   case LEDColor::WHITE:
      name = String("WHITE");
      break;
   case LEDColor::ORANGE:
      name = String("ORANGE");
      break;
   case LEDColor::PURPLE:
      name = String("PURPLE");
      break;
   }
   return name;
}

void loop ( void ) {
   
   // run through the solid colors, solid LED on
   Serial.println(F("\n\n... Starting cycle ... SOLID COLORS ..."));
   led.setState(LEDState::ON);
   for ( auto color : colors ) {
      rgb.setColor(color);                      // after setting color, must set to ON before it is illuminated
      rgb.setState(LEDState::ON);
      Serial.printf("Color: %s (%0X)\n", colorName(rgb.getColor()).c_str(), color);       // just to demo getColor() function
      delay(5000);
      rgb.setState(LEDState::OFF);
   }	
   led.setState(LEDState::OFF);
   delay(5000);

   // same colors, but now blinking on/off in tandem with solid LED
   led.setState(LEDState::BLINK_ON, 500);
   Serial.println(F("\n\n ... BLINKING COLORS ..."));
   for ( auto color : colors ) {
      rgb.setColor(color);
      rgb.setState(LEDState::BLINK_ON, 500);
      Serial.printf("Color: %s (%0X)\n", colorName(rgb.getColor()).c_str(), color);
      delay(5000);
      rgb.setState(LEDState::OFF);
   }
   led.setState(LEDState::OFF);
   delay(5000);

   // alternate between RGB and solid LED
   Serial.println(F("\n\n... ALTERNATING RGB/SOLID ..."));
   Serial.println(F("RGB: RED"));
   rgb.setColor(LEDColor::RED);
   rgb.setState(LEDState::BLINK_ON, 500);             // LEDs will alternate when blinking initiated in opposite states
   led.setState(LEDState::BLINK_OFF, 500);
   delay(5000);
   Serial.println(F("RGB: GREEN"));
   rgb.setColor(LEDColor::GREEN);
   rgb.setState(LEDState::BLINK_ON, 125);            // faster blinking
   led.setState(LEDState::BLINK_OFF, 125);
   delay(5000);
   led.setState(LEDState::OFF);
   rgb.setState(LEDState::OFF);

   // alternating RGB colors
   Serial.println(F("\n\n... ALTERNATING RGB COLORS ..."));
   Serial.println(F("RED/GREEN"));
   rgb.setAlternatingColors(LEDColor::GREEN, LEDColor::RED);
   rgb.setState(LEDState::ALTERNATE, 500);
   delay(5000);
   Serial.println(F("ORANGE/PURPLE"));
   rgb.setAlternatingColors(LEDColor::ORANGE, LEDColor::PURPLE);
   rgb.setState(LEDState::ALTERNATE, 1000);
   delay(5000);
   Serial.println(F("BLUE/MAGENTA"));
   rgb.setAlternatingColors(LEDColor::BLUE, LEDColor::MAGENTA);
   rgb.setState(LEDState::ALTERNATE, 125);
   delay(5000);
   rgb.setState(LEDState::OFF);
   Serial.println(F("\n\n... Cycle complete ...\n"));

   delay(10000);
}