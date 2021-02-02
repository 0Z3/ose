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

/***************************************************************************
  This sketch demonstrates how to call a function written in C/C++
  by sending a message to Ose. Messages that begin with /! represent
  function calls, and when Ose doesn't recognize the remainder of the
  address as the name of a builtin function, it checks the environment.

  All we have to do for this to work, is to add a message to the
  environment for each function that we want to be able to call.
  See the setup() function below, and the three fuunctions: 
  mysleep(), myuniform(), and myled().
***************************************************************************/

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

// WiFi configuration using values defined in ESP32-WiFiUDP-conf.h
const char * const ssid = CONF_MYSSID;
const char * const pass = CONF_MYPASS;
const unsigned int port_local = CONF_PORT_LOCAL;
const unsigned int port_remote = CONF_PORT_REMOTE;
WiFiUDP udp;
boolean connected = false;
char *incoming_packet;

/***************************************************************************
  The following two functions handle the WiFi connection. We call
  connect_to_wifi() from our setup() function, and wifi_event_handler()
  is called by the ESP32 system in response to different events
  such as getting an IP address or getting disconnected.
***************************************************************************/
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

  This sketch doesn't make use of these hooks--see the other sketches 
  for examples of their use.
****************************************************************************/
int my_isKnownAddress(const char * const address)
{
	return 0;
}
void my_default(ose_bundle osevm, char *pattern)
{
	osevm_default(osevm, pattern);
}
void my_assign(ose_bundle osevm, char *address)
{
	osevm_assign(osevm, address);
}
void my_lookup(ose_bundle osevm, char *address)
{
	osevm_lookup(osevm, address);
}

/***************************************************************************
  The next three functions encapsulate some of the basic functionality of the 
  ESP32. We'll store these three functions in the environment, and then
  we'll be able to call them by sending the corresponding address, prefixed
  by /!.
***************************************************************************/

// Cause the system to sleep for an amount of time specified by the
// value on the stack.
void mysleep(ose_bundle osevm)
{
	if(ose_getBundleElemCount(vm_s) > 0
	   && ose_isNumericType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
		if(ose_isIntegerType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
			delay((unsigned long)ose_popInt32(vm_s));
		}else if(ose_isFloatType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
			delay((unsigned long)ose_popFloat(vm_s));
		}
	}
}

// This function uses the ESP32's true random number generator to produce
// a uniform random number between [0,1], which it pushes onto the stack
// in the form of a message with address /u.
void myuniform(ose_bundle osevm)
{
	ose_pushMessage(vm_s, "/u", 2, 1,
			OSETT_FLOAT,
			(float)((double)esp_random() / (double)UINT32_MAX));
}

// Set the state of the onboard LED (you may have to change the value
// of pin_led at the top of this file, if you're not using an
// Adafruit Feather. If there's an int on the stack, use that, otherwise,
// read the current value and set it to the opposite.
void myled(ose_bundle osevm)
{
	int v;
	if(ose_getBundleElemCount(vm_s) > 0
	   && ose_isIntegerType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
		v = ose_popInt32(vm_s) == HIGH ? HIGH : LOW;
	}else{
		v = digitalRead(pin_led) == HIGH ? LOW : HIGH;
	}
	digitalWrite(pin_led, v);
}

void setup() 
{
	Serial.begin(115200);
	pinMode(pin_led, OUTPUT);
	
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

	// Push our functions into the environment. They're just messages,
	// and we can give them whatever names (addresses) we want.
	ose_pushMessage(vm_e, "/sleep", strlen("/sleep"), 1,
			OSETT_CFUNCTION, mysleep);
	ose_pushMessage(vm_e, "/uniform", strlen("/uniform"), 1,
			OSETT_CFUNCTION, myuniform);
	ose_pushMessage(vm_e, "/led", strlen("/led"), 1,
			OSETT_CFUNCTION, myled);

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
