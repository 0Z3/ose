# Ose for Arduino

This folder contains a number of sketches for different platforms that 
demonstrate different paradigms for using Ose on a microcontroller.

## Installation

1. Uncomment (remove /* and /*) the following lines in ose_conf.h
in the top level ose directory:

```
/* #ifndef OSEVM_ASSIGN */
/* #define OSEVM_ASSIGN my_assign */
/* #endif */

/* #ifndef OSEVM_LOOKUP */
/* #define OSEVM_LOOKUP my_lookup */
/* #endif */

...

/* #ifndef OSEVM_DEFAULT */
/* #define OSEVM_DEFAULT my_default */
/* #endif */
...

/* #ifndef OSEVM_ISKNOWNADDRESS */
/* #define OSEVM_ISKNOWNADDRESS my_isKnownAddress */
/* #endif */
```

2. For examples where the device connects to your WiFi network, make sure
you fill in the name of the network and the password in the conf.h file
next to the sketch.

## Requirements

In order to run the Max and PD patches, you will need the odot
objects:
[https://github.com/CNMAT/CNMAT-odot](https://github.com/CNMAT/CNMAT-odot).

For the examples that communicate over serial, the PD patches use
`comport`, available by choosing "Find PD externals" in the application
and searcing for "comport".

## ESP32

The examples in this folder are configured for, and were tested on
Adafruit's 
[ESP32 Feather](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts).
They should work on any ESP32 board, as long as
you make the appropriate changes to the pin numbers.

## ESP8266

The examples in this folder are configured for, and were tested on
Adafruit's 
[ESP8266 Feather](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/pinouts).
They should work on any ESP8266 board, as long as
you make the appropriate changes to the pin numbers.

## Teensy 4.0

The Teensy 4.0 examples demonstrate functionality similar to that of
the ESP* examples, only over serial (USB and UART) instead of WiFi.

## Pico

The pico examples use the pico SDK and CMake for building and
uploading -- please see the documentation here
[https://www.raspberrypi.org/documentation/rp2040/getting-started/]
(https://www.raspberrypi.org/documentation/rp2040/getting-started/).