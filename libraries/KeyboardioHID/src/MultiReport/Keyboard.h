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

#include "HIDAliases.h"
#include "HIDTables.h"

#define KEY_BITS (4 + HID_LAST_KEY - HID_KEYBOARD_A_AND_A + 1)
#define KEY_BYTES ((KEY_BITS + 7) / 8)

// clang-format off
// Keyboard Report Descriptor Template
#define TUD_HID_REPORT_DESC_KEYBOARD_NKRO_DYGMA(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                    ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD )                    ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                    , /* Report ID if any */\
    __VA_ARGS__                                 /* 8 bits Modifier Keys (Shift, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 224                                     ) ,\
      HID_USAGE_MAX    ( 231                                     ) ,\
      HID_LOGICAL_MIN  ( 0                                       ) ,\
      HID_LOGICAL_MAX  ( 1                                       ) ,\
      HID_REPORT_COUNT ( 8                                       ) ,\
      HID_REPORT_SIZE  ( 1                                       ) ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED                   )       ,\
      HID_USAGE_MIN    ( 1                                       ) ,\
      HID_USAGE_MAX    ( 8                                       ) ,\
      HID_REPORT_COUNT ( 8                                       ) ,\
      HID_REPORT_SIZE  ( 1                                       ) ,\
      HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_REPORT_COUNT ( 1                                       ) ,\
      HID_REPORT_SIZE  ( 4                                       ) ,\
      HID_OUTPUT       ( HID_CONSTANT                            ) ,\
      HID_USAGE_MIN    ( HID_KEYBOARD_A_AND_A                    ) ,\
      HID_USAGE_MAX    ( HID_LAST_KEY                            ) ,\
      HID_LOGICAL_MIN  ( 0                                       ) ,\
      HID_LOGICAL_MAX  ( 0x01                                    ) ,\
      HID_REPORT_COUNT ( (KEY_BITS - 4)                          ) ,\
      HID_REPORT_SIZE  ( 0x01                                    ) ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
      HID_REPORT_COUNT ( (8 - (KEY_BITS % 8))                    ) ,\
      HID_REPORT_SIZE  ( 0x01                                    ) ,\
      HID_INPUT        ( HID_CONSTANT                            ) ,\
  HID_COLLECTION_END \
/*Enable back clang*/
// clang-format on
typedef union
{
    // Modifiers + keymap
    struct
    {
        uint8_t modifiers;
        uint8_t keys[KEY_BYTES];
    };
    uint8_t allkeys[1 + KEY_BYTES];
} HID_KeyboardReport_Data_t;


class Keyboard_
{
  public:
    Keyboard_();
    void begin();
    void end();

    size_t press(uint8_t k);
    size_t release(uint8_t k);
    void releaseAll();
    int sendReport();

    bool isKeyPressed(uint8_t k);
    bool wasKeyPressed(uint8_t k);
    bool isModifierActive(uint8_t k);
    bool wasModifierActive(uint8_t k);
    bool isAnyModifierActive();
    bool wasAnyModifierActive();

    uint8_t getLEDs()
    {
        return HID().getLEDs();
    };

  private:
    HID_KeyboardReport_Data_t report_;
    HID_KeyboardReport_Data_t last_report_;

    int sendReportUnchecked();
};
extern Keyboard_ Keyboard;
