/*
Copyright (c) 2014-2015 NicoHood
Copyright (c) 2015-2018 Keyboard.io, Inc

See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

// Include guard
#pragma once

#include "HID-Settings.h"
#include "hidDefy.h"
#include <Arduino.h>

// Dpad directions
#define GAMEPAD_DPAD_CENTERED 0
#define GAMEPAD_DPAD_UP 1
#define GAMEPAD_DPAD_UP_RIGHT 2
#define GAMEPAD_DPAD_RIGHT 3
#define GAMEPAD_DPAD_DOWN_RIGHT 4
#define GAMEPAD_DPAD_DOWN 5
#define GAMEPAD_DPAD_DOWN_LEFT 6
#define GAMEPAD_DPAD_LEFT 7
#define GAMEPAD_DPAD_UP_LEFT 8


typedef union {
  // 32 Buttons, 6 Axis, 2 D-Pads
  uint32_t buttons;

  struct {
    uint8_t button1 : 1;
    uint8_t button2 : 1;
    uint8_t button3 : 1;
    uint8_t button4 : 1;
    uint8_t button5 : 1;
    uint8_t button6 : 1;
    uint8_t button7 : 1;
    uint8_t button8 : 1;

    uint8_t button9 : 1;
    uint8_t button10 : 1;
    uint8_t button11 : 1;
    uint8_t button12 : 1;
    uint8_t button13 : 1;
    uint8_t button14 : 1;
    uint8_t button15 : 1;
    uint8_t button16 : 1;

    uint8_t button17 : 1;
    uint8_t button18 : 1;
    uint8_t button19 : 1;
    uint8_t button20 : 1;
    uint8_t button21 : 1;
    uint8_t button22 : 1;
    uint8_t button23 : 1;
    uint8_t button24 : 1;

    uint8_t button25 : 1;
    uint8_t button26 : 1;
    uint8_t button27 : 1;
    uint8_t button28 : 1;
    uint8_t button29 : 1;
    uint8_t button30 : 1;
    uint8_t button31 : 1;
    uint8_t button32 : 1;

    int16_t	xAxis;
    int16_t	yAxis;

    int16_t	rxAxis;
    int16_t	ryAxis;

    int8_t	zAxis;
    int8_t	rzAxis;

    uint8_t	dPad1 : 4;
    uint8_t	dPad2 : 4;
  };
} HID_GamepadReport_Data_t;

class Gamepad_ {
 public:
  Gamepad_();

  void begin();
  void end();
  void write();
  void press(uint8_t b);
  void release(uint8_t b);
  void releaseAll();

  void buttons(uint32_t b);
  void xAxis(int16_t a);
  void yAxis(int16_t a);
  void zAxis(int8_t a);
  void rxAxis(int16_t a);
  void ryAxis(int16_t a);
  void rzAxis(int8_t a);
  void dPad1(int8_t d);
  void dPad2(int8_t d);

  void sendReport(void* data, int length);
 protected:
  HID_GamepadReport_Data_t report_;
};
extern Gamepad_ Gamepad;
