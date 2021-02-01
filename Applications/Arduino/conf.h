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
  Ose for Arduino

  This file contains configuration for both the Ose system, 
  and the Arduino sketches. 
*/

/***************************************************************************
 * Edit these to work with your wireless network
 ***************************************************************************/
#define CONF_MYSSID "FRITZ!Box 7530 BS"
#define CONF_MYPASS "18096912605906665686"

/***************************************************************************
 * If you change these, you'll have to update the Max / PD patches included
 * with the sketches.
 * 
 * Note: since this code is running on a microcontroller, "local" and 
 * "remote" are from the perspective of the microcontroller. 
 ***************************************************************************/
#define CONF_PORT_LOCAL 10000
#define CONF_PORT_REMOTE 10001

/***************************************************************************
 * This is the amount of memory that Ose will use. This value is 
 * be more than enough to handle the examples, and should fit comfortably
 * on the different platforms. You shouldn't have to edit this.
 ***************************************************************************/
#define CONF_BUNDLE_SIZE 65536

/***************************************************************************
 * These configure the Ose system and its VM. 
 * You shouldn't have to change anything below here.
 ***************************************************************************/
#define OSE_CONF_VM_INPUT_SIZE CONF_BUNDLE_SIZE / 8
#define OSE_CONF_VM_STACK_SIZE CONF_BUNDLE_SIZE / 8
#define OSE_CONF_VM_ENV_SIZE CONF_BUNDLE_SIZE / 8
#define OSE_CONF_VM_CONTROL_SIZE CONF_BUNDLE_SIZE / 8
#define OSE_CONF_VM_DUMP_SIZE CONF_BUNDLE_SIZE / 8
#define OSE_CONF_VM_OUTPUT_SIZE CONF_BUNDLE_SIZE / 8

#define OSEVM_DEFAULT respond
#define OSEVM_ISKNOWNADDRESS canrespond
#define OSEVM_ASSIGN assign
#define OSEVM_LOOKUP lookup
