# Ose for Arduino

This folder contains a number of sketches for different platforms that 
demonstrate different paradigms for using Ose on a microcontroller.

## Installation

1. In the top level ose directory, copy ose_conf.h.example and name it
ose_conf.h. You do not need to edit this file.

1. For each of the Arduino sketches you'd like to run, make sure there's
a copy of the appropriate conf file in its folder. For example, to run
one of the ESP32-WiFiUDP sketches, make sure you've edited the 
ESP32-WiFiUDP-conf.h file, and copied it into the folder for the sketch
you want to run.

## ESP32

The examples in this folder are configured for, and were tested on
Adafruit's 
[ESP32 Feather](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts).
They should work on any ESP32 board, as long as
you make the appropriate changes to the pin numbers.

These three sketches are nearly identical to one another---only differing
in which functions are implemented.

### ESP32-WiFiUDP-AddressServer

This sketch mimics some of the basic functionality of OSCuino
[https://github.com/CNMAT/OSC](https://github.com/CNMAT/OSC). It demonstrates
the use of two hooks to respond to arbitrary OSC addresses.

### ESP32-WiFiUDP-AssignmentLookup

This sketch extends Ose's assignment and lookup messages (/@ and /$)
to support "assigning" values to pins, and "looking up" the value of pins.

### ESP32-WiFiUDP-FunctionBinding

This sketch demonstrates binding a C / C++ function into the Ose environment
so that it can be called by sending a message with the function call prefix (/!).
