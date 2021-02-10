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
  This sketch mimics some of the basic functionality of OSCunio 
  (https://github.com/CNMAT/OSC): messages of the form
  /d/<n> refer to digital pin <n>, and /a/<n> refer to analog pin <n>.

  When /d/<n> messages have an argument, we'll set pin <n> to that value
  using digitalWrite(), and when they don't have an argument, we'll read
  the value of the pin and send it back. Since there's no analogWrite()
  function for the ESP32, whenever we receive a /a/<n> message, we'll
  read the value using analogRead() and send it back.

  Some examples:
	/d/13 : 1	set digital pin 13 to HIGH
	/a/2		get the value of analog pin 2
	/d/*  : 0	set the value of all digital pins to LOW
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

#include "conf.h"

#define pin_led 13

// These are the digital addresses we respond to
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
IPAddress ip_local;
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
	Serial.print("Connected! IP address: ");
	Serial.println(ip_local);
	Serial.printf("UDP server on port %d\n", port_local);
}

// A pointer to the raw bytes that Ose will use as its bundle
char *bytes;
// The bundle, VM, Input, Stack, Environment, and Output
ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_o;

/****************************************************************************
  The next four functions are "hooks" that the Ose VM will call at different
  points during the processing of a bundle. These are installed in the Ose 
  system at compile time by the preprocessor directives in conf.h.

  In this sketch, we implement the first two; the second two are used in
  other sketches.
****************************************************************************/

/*
  When Ose encounters an address or a string that looks like an address,
  it checks to see if it responds to it, for example, it responds to addresses
  that begin with "/@", but not to the address "/foo". For addresses that it
  does not respond to, it will call this function (if's been installed as
  a hook). 

  If this function returns a 1, the next function below will be called when 
  it is time to actually respond to the address.

  In our case, we want to respond to any messages that begin with the
  three characters /a/ or /d/.
*/
int my_isKnownAddress(const char * const address)
{
	if((address[0] == '/' && address[1] == 'a' && address[2] == '/')
	   || (address[0] == '/' && address[1] == 'd' && address[2] == '/')){
		return 1;
	}
	return 0;
}

/*
  For any address that Ose does not respond to, this function will be called
  as long as my_isKnownAddress()  returned a 1.

  The first argument is the entire VM, and pattern is the address that
  this function should respond to.
 */
void my_default(ose_bundle osevm, char *pattern)
{
	// if this is not an address we respond to,
	// call Ose's default hook and bail out
	if(!(pattern[0] == '/' && pattern[1] == 'a' && pattern[2] == '/')
	   && !(pattern[0] == '/' && pattern[1] == 'd' && pattern[2] == '/')){
		osevm_default(osevm, pattern);
		return;
	}

	if(pattern[1] == 'a'){
		// Since there isn't an analogWrite() function for the ESP32,
		// we'll just read the value of the pin(s)

		// Loop over all the pin addresses we respond to
		for(int i = 0;
		    i < sizeof(analog_input_addresses) / sizeof(char *);
		    i++){
			const char * const addr =
				analog_input_addresses[i];
			const int pin = analog_input_pins[i];
			int po = 0, ao = 0;
			int r = ose_match_pattern(pattern, addr, &po, &ao);
			if(r & OSE_MATCH_ADDRESS_COMPLETE){
				// The entirety of addr was matched.
				// Read the corresponding pin and push a message
				// into the output bundle with the address
				// and value.
				pinMode(pin, INPUT);
				float v = analogRead(pin);
				ose_pushMessage(vm_o,
						addr,
						strlen(addr),
						1, OSETT_FLOAT, v);
			}
		}
	}else{ // 'd'
		// For digital pins, we can either read or write. 
		if(ose_getBundleElemCount(vm_s)
		   && ose_isIntegerType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
			// There's an integer on the stack, so write LOW
			// to the appropriate pin(s) if it's 0,
			// or HIGH otherwise.
			int32_t v = ose_popInt32(vm_s) == 0 ? LOW : HIGH;
			for(int i = 0;
			    i < sizeof(digital_output_addresses) / sizeof(char *);
			    i++){
				const char * const addr =
					digital_output_addresses[i];
				const int pin = digital_output_pins[i];
				int po = 0, ao = 0;
				int r = ose_match_pattern(pattern,
							  addr,
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					pinMode(pin, OUTPUT);
					digitalWrite(pin, v);
				}
			}
		}else{
			// There's nothing on the stack (or there's something,
			// but it's not an int), so we'll read the value
			// of the pin(s) and put it in the output bundle.
			for(int i = 0;
			    i < sizeof(digital_input_addresses) / sizeof(char *);
			    i++){
				const char * const addr =
					digital_input_addresses[i];
				const int pin = digital_input_pins[i];
				int po = 0, ao = 0;
				int r = ose_match_pattern(pattern,
							  addr,
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					pinMode(pin, INPUT);
					int32_t v = digitalRead(pin) == HIGH ? 1 : 0;
					ose_pushMessage(vm_o,
							addr,
							strlen(addr),
							1, OSETT_INT32, v);
				}
			}
		}
	}
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

void myping(ose_bundle osevm)
{
	ose_pushMessage(vm_o, "/ping", 5, 4,
			OSETT_INT32, ip_local[0],
			OSETT_INT32, ip_local[1],
			OSETT_INT32, ip_local[2],
			OSETT_INT32, ip_local[3]);
}

#define VM_BUNDLE_SIZE 8192
void setup() 
{
	Serial.begin(115200);

	// Allocate memory for our bundle. The VM consists of 6 bundles,
	// which can be any size. The total amount of memory used by the
	// VM is equal to the size of those 6 bundles, plus som extra space
	// for housekeeping. The following function computes exactly the
	// amount of space we need, given the sizes of the 6 bundles.
	// Note: these sizes can be burned into the VM more efficiently
	// at compile time by editing ose_conf.h.
	int32_t n = osevm_computeSizeReqs(VM_BUNDLE_SIZE, VM_BUNDLE_SIZE,
					  VM_BUNDLE_SIZE, VM_BUNDLE_SIZE,
					  VM_BUNDLE_SIZE, VM_BUNDLE_SIZE, 0);
	Serial.printf("Allocating %d bytes for bundle\n", n);
	bytes = (char *)malloc(n);
	if(!bytes){
		Serial.printf("Couldn't allocate %d bytes for bundle!\n\r", n);
		return;
	}
	// Allocate space to receive an incoming bundle over UDP
	incoming_packet = (char *)malloc(VM_BUNDLE_SIZE);
	if(!incoming_packet){
		Serial.printf("Couldn't allocate %d bytes for incoming packet!\n\r",
			      VM_BUNDLE_SIZE);
		return;
	}

	// Initialize our bundle
	bundle = ose_newBundleFromCBytes(n, bytes);
	// Initialize the VM
	osevm = osevm_init(bundle,
			   VM_BUNDLE_SIZE, VM_BUNDLE_SIZE, VM_BUNDLE_SIZE,
			   VM_BUNDLE_SIZE, VM_BUNDLE_SIZE, VM_BUNDLE_SIZE);
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
	ose_pushMessage(vm_e, "/ping", strlen("/ping"), 1,
			OSETT_CFUNCTION, myping);

	// Finally, connect to the WiFi network
	connect_to_wifi(ssid, pass);
}

void loop() 
{
	int size = udp.parsePacket();
	if(size){
		// Read the UDP packet, and save the address it was received
		// from so that we can send a packet back to it
		udp.read(incoming_packet, VM_BUNDLE_SIZE);
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
