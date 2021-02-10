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
