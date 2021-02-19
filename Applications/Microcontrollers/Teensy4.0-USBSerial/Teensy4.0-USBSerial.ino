/*
  Copyright (c) 2019-21 John MacCallum
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

// For access to Teensy's true random number generator
#include <Entropy.h>

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
#include <ose_print.h>

#define pin_led 13

// These are the digital addresses we respond to
const char * const digital_addresses[] =
	{
		"/d/0",  "/d/1",  "/d/2",  "/d/3",
		"/d/4",  "/d/5",  "/d/6",  "/d/7",
		"/d/8",  "/d/9",  "/d/10", "/d/11",
		"/d/12", "/d/13", "/d/14", "/d/15",
		"/d/16", "/d/17", "/d/18", "/d/19",
		"/d/20", "/d/21", "/d/22", "/d/23",
		"/d/24", "/d/25", "/d/26", "/d/27",
		"/d/28", "/d/29", "/d/30", "/d/31",
		"/d/32", "/d/33", "/d/34", "/d/35",
		"/d/36", "/d/37", "/d/38", "/d/39",
	};

// These are the analog input addresses we respond to
const char * const analog_addresses[] =
	{
		"/a/0",  "/a/1",  "/a/2",  "/a/3",
		"/a/4",  "/a/5",  "/a/6",  "/a/7",
		"/a/8",  "/a/9",  "/a/10", "/a/11",
		"/a/12", "/a/13", 
	};

// These are the analog output (PWM) addresses we respond to
const char * const pwm_addresses[] =
	{
		"/a/0",  "/a/1",  "/a/2",  "/a/3",
		"/a/4",  "/a/5",  "/a/6",  "/a/7",
		"/a/8",  "/a/9",  "/a/10", "/a/11",
		"/a/12", "/a/13", "/a/14", "/a/15",
		/*"/a/16", "/a/17",*/ "/a/18", "/a/19",
		/*"/a/20", "/a/21",*/ "/a/22", "/a/23",
		"/a/24", "/a/25", /*"/a/26", "/a/27",*/
		"/a/28", "/a/29", /*"/a/30", "/a/31",*/
		/*"/a/32",*/ "/a/33", "/a/34", "/a/35",
		"/a/36", "/a/37", "/a/38", "/a/39",
	};

const int pwm_pins[] =
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		18, 19, 22, 23, 24, 25, 28, 29,
		33, 34, 35, 36, 37, 38, 39
	};

char *incoming_packet;

// A pointer to the raw bytes that Ose will use as its bundle
char *bytes;
// The bundle, VM, Input, Stack, Environment, and Output
ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_o;
int echo;

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
	   || (address[0] == '/' && address[1] == 'd' && address[2] == '/')
	   || (address[0] == '/' && address[1] == 0)){
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
	   && !(pattern[0] == '/' && pattern[1] == 'd' && pattern[2] == '/')
	   && !(pattern[0] == '/' && pattern[1] == 0)){
		osevm_default(osevm, pattern);
		return;
	}
	if(pattern[1] == 0){
		echo = echo ? 0 : 1;
		if(echo){
			Serial1.printf("(echoing input)\n\r");
		}
		return;
	}

	if(pattern[1] == 'a'){
		if(ose_getBundleElemCount(vm_s) > 0
		   && ose_isIntegerType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
			// write
			int32_t v = ose_peekInt32(vm_s);
			int foundmatch = 0;
			for(int i = 0;
			    i < sizeof(pwm_addresses) / sizeof(char*);
			    i++){
				int po, ao;
				int r = ose_match_pattern(pattern,
							  pwm_addresses[i],
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					foundmatch = 1;
					analogWrite(pwm_pins[i], v);
				}
			}
			if(foundmatch){
				ose_drop(vm_s);
			}
		}else{
			// read
			for(int i = 0;
			    i < sizeof(analog_addresses) / sizeof(char*);
			    i++){
				int po, ao;
				int r = ose_match_pattern(pattern,
							  analog_addresses[i],
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					pinMode(i + 14, INPUT);
					float v = analogRead(i + 14);
					ose_pushMessage(vm_o,
							analog_addresses[i],
							strlen(analog_addresses[i]),
							1, OSETT_FLOAT, v);
				}
			}
		}
	}else{ // 'd'
		if(ose_getBundleElemCount(vm_s) > 0
		   && ose_isIntegerType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
			// write
			int32_t v = ose_peekInt32(vm_s) == 0 ? LOW : HIGH;
			int foundmatch = 0;
			for(int i = 0;
			    i < sizeof(digital_addresses) / sizeof(char*);
			    i++){
				int po, ao;
				int r = ose_match_pattern(pattern,
							  digital_addresses[i],
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					foundmatch = 1;
					pinMode(i, OUTPUT);
					digitalWrite(i, v);
				}
			}
			if(foundmatch){
				ose_drop(vm_s);
			}
		}else{
			// read
			for(int i = 0;
			    i < sizeof(digital_addresses) / sizeof(char*);
			    i++){
				int po, ao;
				int r = ose_match_pattern(pattern,
							  digital_addresses[i],
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					pinMode(i, INPUT);
					int32_t v = digitalRead(i) == HIGH ? 1 : 0;
					ose_pushMessage(vm_o,
							digital_addresses[i],
							strlen(digital_addresses[i]),
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
			    i < sizeof(digital_addresses) / sizeof(char*);
			    i++){
				const char * const addr =
					digital_addresses[i];
				int po = 0, ao = 0;
				int r = ose_match_pattern(address + 2,
							  addr,
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					pinMode(i, OUTPUT);
					digitalWrite(i, v ? HIGH : LOW);
					foundmatch = 1;
				}
			}
			if(foundmatch){
				ose_drop(vm_s);
			}
		}
	}else if(address[2] == '/' && address[3] == 'a' && address[4] == '/'){
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
			    i < sizeof(pwm_addresses) / sizeof(char*);
			    i++){
				const char * const addr =
					pwm_addresses[i];
				int po = 0, ao = 0;
				int r = ose_match_pattern(address + 2,
							  addr,
							  &po, &ao);
				if(r & OSE_MATCH_ADDRESS_COMPLETE){
					analogWrite(pwm_pins[i], v);
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
		    i < sizeof(digital_addresses) / sizeof(char*);
		    i++){
			int po = 0, ao = 0;
			int r = ose_match_pattern(address + 2,
						  digital_addresses[i],
						  &po, &ao);
			if(r & OSE_MATCH_ADDRESS_COMPLETE){
				pinMode(i, INPUT);
				int v = digitalRead(i)
					== HIGH ? 1 : 0;
				// push the value of the pin onto the stack
				ose_pushInt32(vm_s, v);
			}
		}
	}else if(address[3] == 'a' && address[4] == '/'){
		for(int i = 0;
		    i < sizeof(analog_addresses) / sizeof(char*);
		    i++){
			int po = 0, ao = 0;
			int r = ose_match_pattern(address + 2,
						  analog_addresses[i],
						  &po, &ao);
			if(r & OSE_MATCH_ADDRESS_COMPLETE){
				pinMode(i + 14, INPUT);
				float v = analogRead(i + 14);
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
			Entropy.randomf());
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

struct ose_SLIPBuf slipbuf;
#define VM_BUNDLE_SIZE 8192
// char incoming_packet[VM_BUNDLE_SIZE];
// char bytes[65536];
void setup() 
{
	Serial1.begin(115200);
	pinMode(pin_led, OUTPUT);
	Entropy.Initialize();
	
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

	//Serial.printf("Allocating %d bytes for bundle\n", n);
	bytes = (char *)malloc(n);
	if(!bytes){
		// Turn the LED on if there was an error
		digitalWrite(pin_led, HIGH);
		return;
	}
	// Allocate space to receive an incoming bundle over UDP
	incoming_packet = (char *)malloc(VM_BUNDLE_SIZE);
	if(!incoming_packet){
		// Turn the LED on if there was an error
		digitalWrite(pin_led, HIGH);
		return;
	}
	
	// Initialize the SLIP buffer
	slipbuf = ose_initSLIPBuf((unsigned char *)incoming_packet,
				  VM_BUNDLE_SIZE);

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

	echo = 0;
}

void loop() 
{
	while(Serial1.available() > 0){
		char c = Serial1.read();
		int r = ose_SLIPDecode(c, &slipbuf);
		if(echo){
			switch(c){
			case 10:
			case 13:
				Serial1.write("\n\r", 2);
				break;
			case OSE_SLIP_END:
				echo = 0;
				break;
			default:
				Serial1.write(c);
			}
			Serial1.flush();
		}
		if(r == -1){
			slipbuf = ose_initSLIPBuf((unsigned char *)incoming_packet,
						  VM_BUNDLE_SIZE);
		}else if(r == 0){
			// Copy the packet we just got into the VM's input bundle
			if(!strncmp(slipbuf.buf, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN)){
				osevm_inputMessages(osevm, slipbuf.count, slipbuf.buf);
			}else{
				for(int i = 0; i < slipbuf.count; i++){
					Serial1.printf("%d ", slipbuf.buf[i]);
				}
				Serial1.printf("\r\n");
				osevm_inputMessage(osevm, slipbuf.count, slipbuf.buf);
			}
			// Run the vm
			osevm_run(osevm);
			if(echo){
				char buf[2048];
				int32_t n = ose_pprintFullBundle_impl(vm_s, buf, 2048, "STACK");
				Serial1.write(buf, n);
			}else{
				// Use the incoming_packet buffer as a temporary
				// buffer to SLIP encode the output bundle of the VM
				memset(incoming_packet, 0, VM_BUNDLE_SIZE);
				int32_t n = ose_SLIPEncode(vm_o,
							   ose_readInt32(vm_o, -4),
							   incoming_packet,
							   VM_BUNDLE_SIZE);
				// Write the SLIP encoded bundle
				Serial1.write(incoming_packet, n);
				Serial1.flush();
			}
			// Clear the stack and the output bundle
			ose_clear(vm_o);
			//ose_clear(vm_s);
			// Reinitialize the SLIP buffer
			slipbuf = ose_initSLIPBuf((unsigned char *)incoming_packet,
						  VM_BUNDLE_SIZE);
		}else{}
	}
}
