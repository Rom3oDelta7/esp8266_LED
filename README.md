# LED library for ESP8266
This library provides functions to manage solid-color (2-lead) LEDs, as well as 3-color RGB (4-lead) LEDs.
Both common cathode and common anode RGB LEDs are supported.
For a common cathode LED, the MCU is a current source and you set each pin connected to the R, G, and B leads HIGH to turn them on.
The LED has a common ground.
With a common anode LED, the MCU is a current sink.
You have a common anode that is set to Vcc, and you set each pin connected to the R, G, and B leads LOW to turn the LED on.
In both cases, be sure to have a current limiting resistor on each of the red, green, and blue lines between the MCU (or ground) and the LED.
(See [LEDCALC] for an online tool to help you determine the current resistor to use).
This library is written specifically for the ESP8266 and uses the SDK OS Timer for blinking LEDs.

This library supercedes the [LED3] library.
It provides all of the functionality of the [LED3] library plus new features, but, unlike [LED3], it can only be used on an ESP8266.

# Programming Interface
## Constructors
```C++
LED (const uint8_t pin);
```

This instantiates a single-color LED using the specified pin.
The pin is set to ```OUTPUT``` mode by the library.

```C++
LED (const uint8_t redPin, const uint8_t greenPin, const uint8_t bluePin, LEDType ledType = LEDType::CATHODE);
```

This instantiates an RGB LED using the specified red, green, and blue pins.
The fourth paramater is optional, and specifies the type of RGB LED.
The default (no argument supplied) is a common cathode type LED.
If the LED has a common anode, the optional parameter should be ```LEDType::ANODE```.
Each pin is set to ```OUTPUT``` mode by the library.

## RGB LED Colors
RGB LED colors consist of a 24-bit value,
with the most significant byte (byte 2) representing red, byte 1 representing green, and the least significant byte (byte 0) representing blue.
The following colors are predefined.
Note that the colors are declared within a scoped enumeration to avoid naming conflicts.
Each color must be entered as shown below.

|Color|Hexadecimal value|
|---|---|
|LEDColor::NONE|0x000000|
|LEDColor::RED|0xFF0000|
|LEDColor::GREEN|0x00FF00|
|LEDColor::BLUE|0x0000FF|
|LEDColor::CYAN|0x00FFFF|
|LEDColor::WHITE|0xFFFFFF|
|LEDColor::ORANGE|0xFF8600|
|LEDColor::PURPLE|0x7700FF|

Other combinations may be passed as a color to the functions below by using a static or C-style cast.
For example:
```C++
led.setColor(LEDColor::GREEN);
uint32_t value = 0x772017;
led.setColor(static_cast<LEDColor>(value));           // C++ cast
led.setColor((uint32_t)value);                        // C-style cast
```

## Functions

```C++
void setColor(const LEDColor color);
```

Set the color of an RGB LED to the given color.
See the previous section for pre-defined colors or setting your own value.
Setting the color does _not_ illuminate the LED;
it must be explicitly turned on via a call to ```setState``` as described below.

```C++
void setAlternatingColors(const LEDColor color1, const LEDColor color2);
```

This function is used to define two alternating colors for an RGB LED.
As with ```setColor``` this only defines the colors to be used and will not turn on the LED.

```C++
LEDColor getColor(void);
```

Returns the current color of an RGB LED.

```C++
void setState(const LEDState ledState, const uint32_t interval = 500);
```

This function changes the state of a single-color or RGB LED.
The state change has immediate effect.
Available states are as follows:

|State|Action|
|---|---|
|LEDState::ON|Turns the LED on|
|LEDState::OFF|Turns the LED off|
|LEDState::BLINK_ON|Starts blinking the LED, with the initial state set to ON|
|LEDState::BLINK_OFF|Starts blinking the LED, with the initial state set to OFF|
|LEDState::ALTERNATE|Starts alternating the color of an RGB LED between the two previously set values|

The _interval_ parameter is optional.
It is only meaningful for LEDs that are blinking or alternating.
The value is specified in milliseconds, and the default is 500ms.

# Examples
1. _LightCycle.ino_: Demonstrates the different modes for single-color and RGB LEDs.
This example demonstreates how to alternate between two LEDs or use 2 alternating colors on a single RGB LED.
2. _ChooseColor.ino_: Reads a 24-bit heaxadecimal color code from the serial port and displays that color.
See this example for how to set arbitrary color values other than the pre-defined ones.

# Installation

This library is also maintained in GitHub, which has a release mechanism.
The easiest way to install this is to get the [Latest Release] and install it using the Arduino IDE Library Manager.

Of course, you may also clone this repository and manually install the files if you prefer.

# Copyright Notice

Copyright 2017 Rob Redford.
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
To view a copy of this license, visit [BY-NC-SA].

[Latest Release]: https://github.com/Rom3oDelta7/esp8266_LED/releases/latest
[LEDCALC]: http://ledcalc.com/
[LED3]: https://github.com/Rom3oDelta7/LED3