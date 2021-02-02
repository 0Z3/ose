/*
  Copyright (c) 2019-20 John MacCallum
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/*
  This sketch overrides Ose's default assignment function, extending it to 
  allow us to "assign" a value to a pin. Similarly, we substitute our own
  lookup function that can "lookup" pin values.
  
  We follow the same conventions as OSCuino (https://github.com/CNMAT/OSC),
  as we did in the sketch ESP-WiFiUDP-AddressServer.ino, where /d/<n> refers
  to digital pin <n>, and /a/<n> refers to analog pin <n>. 

  In this example, though, we will use the Ose prefixes for assignment and 
  lookup: /@ and /$. Since we don't have an analogWrite function for the 
  ESP32, we will only read ("lookup") from the analog pins.

  Unlike ESP32-WiFiUDP-AddressServer.ino, when we do a lookup here, the value
  of the pin will be placed on the stack, just like Ose's normal behavior.
  That way, it will be available for further processing by other messages
  we might send. If we want the value to be sent back to us, we'll have to
  explicitly put it in the output bundle (see the Max / PD patches).

  Examples:
	/@/d/* 			set all digital pins to the value on the stack
	/@/d/{25,26} : 0	set digital pins 25 and 26 to LOW
	/$/a/[79]		push the values of analog pins 7 and 9 onto the stack
*/

// Standard Arduino libs for working with UDP over WiFi on the ESP32
#include "esp_system.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

// Ose includes (ose.h should be first)
#include <ose.h>
// Basic Ose functionality for manipulating OSC
#include <ose_context.h>
#include <ose_util.h>
#include <ose_stackops.h>
// Support for Ose's VM
#include <ose_vm.h>
// Support for pattern matching OSC addresses
#include <ose_match.h>

#include "ESP32-WiFiUDP-conf.h"

#define pin_led 13

// These are the digital addresses we respond to that produce values
const char * const digital_input_addresses[] =
	{
		"/d/25", "/d/26", "/d/34", "/d/39",
		"/d/36", "/d/4",  "/d/5",  "/d/18",
		"/d/19", "/d/16", "/d/17", "/d/21",
		"/d/13", "/d/12", "/d/27", "/d/33",
		"/d/15", "/d/32", "/d/14", "/d/22",
		"/d/23"
	};
// These are the pin numbers for each digital input
const int digital_input_pins[] =
	{
		25, 26, 34, 39,
		36, 4,  5,  18,
		19, 16, 17, 21,
		13, 12, 27, 33,
		15, 32, 14, 22,
		23
	};

// Some of the pins on the ESP32 can be used for input, but not output
const char * const digital_output_addresses[] =
	{
		"/d/25", "/d/26",
		         "/d/4",  "/d/5",  "/d/18",
		"/d/19", "/d/16", "/d/17", "/d/21",
		"/d/13", "/d/12", "/d/27", "/d/33",
		"/d/15", "/d/32", "/d/14", "/d/22",
		"/d/23"
	};
// These are the pin numbers for each digital output
const int digital_output_pins[] =
	{
		25, 26, 
		    4,  5,  18,
		19, 16, 17, 21,
		13, 12, 27, 33,
		15, 32, 14, 22,
		23
	};

// Analog pin addresses
const char * const analog_input_addresses[] =
	{
		"/a/2",  "/a/3",
		"/a/4",  "/a/7",
		"/a/9",  "/a/11",
	};

// Analog pins
const int analog_input_pins[] =
	{
		A2, A3, A4, A7, A9, A11
	};

// WiFi configuration using values defined in ESP32-WiFiUDP-conf.h
const char * const ssid = CONF_MYSSID;
const char * const pass = CONF_MYPASS;
const unsigned int port_local = CONF_PORT_LOCAL;
const unsigned int port_remote = CONF_PORT_REMOTE;
WiFiUDP udp;
boolean connected = false;
char *incoming_packet;

/*
  The following two functions handle the WiFi connection. We call
  connect_to_wifi() from our setup() function, and wifi_event_handler()
  is called by the ESP32 system in response to different events
  such as getting an IP address or getting disconnected.
*/
void wifi_event_handler(WiFiEvent_t e)
{
	IPAddress ip_local;
	switch(e){
	case SYSTEM_EVENT_STA_GOT_IP:
		ip_local = WiFi.localIP();
		udp.begin(ip_local, port_local);
		connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		connected = false;
		break;
	}
}

void connect_to_wifi(const char *ssid, const char *pass)
{
	WiFi.disconnect(true);
	WiFi.onEvent(wifi_event_handler);
	WiFi.begin(ssid, pass);
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}
}

#define CONF_BUNDLE_SIZE 65536
// A pointer to the raw bytes that Ose will use as its bundle
char *bytes;
// The bundle, VM, Input, Stack, Environment, and Output
ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_o;

/****************************************************************************
  The next four functions are "hooks" that the Ose VM will call at different
  points during the processing of a bundle. These are installed in the Ose 
  system at compile time by the preprocessor directives in conf.h
****************************************************************************/

/*
  These two hooks aren't implemented here; see ESP32-WiFiUDP-AddressServer.ino
  for an example of their use.
*/
int my_isKnownAddress(const char * const address)
{
	return 0;
}

void my_default(ose_bundle osevm, char *pattern)
{
	osevm_default(osevm, pattern);
}

/*
  This function will be called anytime Ose processes an address or a string
  that begins with the prefix /@. If the next part of the address is /d/,
  we'll try to treat it as an assignment to a digital pin. If not, we'll call
  Ose's builtin assignment function, which will perform a normal assignment
  into the environment.
*/
void my_assign(ose_bundle osevm, char *address)
{
	// we only care about messages that look like /@/d/
	if(address[2] == '/' && address[3] == 'd' && address[4] == '/'){
		// if there's nothing on the stack, then there's nothing to do
		if(ose_isIntegerType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
			// get the value on top of the stack
			int v = ose_peekInt32(vm_s);
			// iterate over all the pin addresses we respond to,
			// and assign the value to any that match the pattern
			// we received. if we assign to at least 1 pin,
			// we'll remove the value from the stack, but if
			// we didn't match any pins, we'll leave it there
			int foundmatch = 0;
			for(int i = 0;
			    i < sizeof(digital_output_pins) / sizeof(int);
			    i++){
				const char * const addr =
					digital_output_addresses[i];
				const int pin = digital_output_pins[i];
				int po = 0, ao = 0;
				int r = ose_match_pattern(address + 2,
							  addr,
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					pinMode(pin, OUTPUT);
					digitalWrite(pin, v ? HIGH : LOW);
					foundmatch = 1;
				}
			}
			if(foundmatch){
				ose_drop(vm_s);
			}
		}
	}else{
		// the assignment wasn't to a pin, so let Ose handle it
		osevm_assign(osevm, address);
	}
}

/*
  Similar to the function above, this will be called anytime Ose processes
  an address or a string that begins with the prefix /$. If the next part of 
  the address is /d/ or /a/, we'll read the value of the corresponding 
  analog or digital pin(s) and leave the value(s) on the stack. If not, we'll
  call Ose's builtin lookup function, which will perform a normal lookup in 
  the environment.
*/
void my_lookup(ose_bundle osevm, char *address)
{
	if(address[2] == '/' && address[3] == 'd' && address[4] == '/'){
		for(int i = 0;
		    i < sizeof(digital_input_addresses) / sizeof(char*);
		    i++){
			int po = 0, ao = 0;
			int r = ose_match_pattern(address + 2,
						  digital_input_addresses[i],
						  &po, &ao);
			if(r & OSE_MATCH_ADDRESS_COMPLETE){
				pinMode(digital_input_pins[i], INPUT);
				int v = digitalRead(digital_input_pins[i])
					== HIGH ? 1 : 0;
				// push the value of the pin onto the stack
				ose_pushInt32(vm_s, v);
			}
		}
	}else if(address[3] == 'a' && address[4] == '/'){
		for(int i = 0;
		    i < sizeof(analog_input_addresses) / sizeof(char*);
		    i++){
			int po = 0, ao = 0;
			int r = ose_match_pattern(address + 2,
						  analog_input_addresses[i],
						  &po, &ao);
			if(r & OSE_MATCH_ADDRESS_COMPLETE){
				pinMode(analog_input_pins[i], INPUT);
				float v = analogRead(analog_input_pins[i]);
				// push the value of the pin onto the stack
				ose_pushFloat(vm_s, v);
			}
		}
	}else{
		// the lookup wasn't for a pin, so let Ose handle it.
		osevm_lookup(osevm, address);
	}
}

void setup() 
{
	Serial.begin(115200);

	// Allocate memory for our bundle 
	bytes = (char *)malloc(CONF_BUNDLE_SIZE);
	if(!bytes){
		Serial.printf("Couldn't allocate %d bytes for bundle!\n\r",
			      CONF_BUNDLE_SIZE);
		return;
	}
	// Allocate space to receive an incoming bundle over UDP
	incoming_packet = (char *)malloc(CONF_BUNDLE_SIZE);
	if(!incoming_packet){
		Serial.printf("Couldn't allocate %d bytes for incoming packet!\n\r",
			      CONF_BUNDLE_SIZE);
		return;
	}

	// Initialize our bundle
	bundle = ose_newBundleFromCBytes(CONF_BUNDLE_SIZE, bytes);
	// Initialize the VM
	osevm = osevm_init(bundle);
	// Store pointers to some of the VM's bundles that we may want to use
	vm_i = OSEVM_INPUT(osevm);
	vm_s = OSEVM_STACK(osevm);
	vm_e = OSEVM_ENV(osevm);
	vm_o = OSEVM_OUTPUT(osevm);

	// Finally, connect to the WiFi network
	connect_to_wifi(ssid, pass);
}

void loop() 
{
	int size = udp.parsePacket();
	if(size){
		// Read the UDP packet, and save the address it was received
		// from so that we can send a packet back to it
		udp.read(incoming_packet, CONF_BUNDLE_SIZE);
		IPAddress ip_remote = udp.remoteIP();

		// Copy the packet we just got into the VM's input bundle
		osevm_inputTopLevel(osevm, size, incoming_packet);
		// Run the vm
		osevm_run(osevm);
		// Send the output bundle to the address we received
		// the packet from
		udp.beginPacket(ip_remote, port_remote);
		udp.write((const uint8_t *)ose_getBundlePtr(vm_o),
			  ose_readInt32(vm_o, -4));
		udp.endPacket();
		// Clear the stack and the output bundle
		ose_clear(vm_s);
		ose_clear(vm_o);
	}
	yield();
}
