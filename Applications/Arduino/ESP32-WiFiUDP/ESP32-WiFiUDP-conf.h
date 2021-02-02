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
#define CONF_MYSSID 
#define CONF_MYPASS

/***************************************************************************
 * If you change these, you'll have to update the Max / PD patches included
 * with the sketches.
 * 
 * Note: since this code is running on a microcontroller, "local" and 
 * "remote" are from the perspective of the microcontroller. 
 ***************************************************************************/
#define CONF_PORT_LOCAL 10000
#define CONF_PORT_REMOTE 10001


